static struct nfs_page *nfs_try_to_update_request(struct inode *inode,
		struct page *page,
		unsigned int offset,
		unsigned int bytes)
{
	struct nfs_page *req;
	unsigned int rqend;
	unsigned int end;
	int error;

	if (!PagePrivate(page))
		return NULL;

	end = offset + bytes;
	spin_lock(&inode->i_lock);

	for (;;) {
		req = nfs_page_find_request_locked(NFS_I(inode), page);
		if (req == NULL)
			goto out_unlock;

		rqend = req->wb_offset + req->wb_bytes;
		/*
		 * Tell the caller to flush out the request if
		 * the offsets are non-contiguous.
		 * Note: nfs_flush_incompatible() will already
		 * have flushed out requests having wrong owners.
		 */
		if (offset > rqend
		    || end < req->wb_offset)
			goto out_flushme;

		if (nfs_lock_request(req))
			break;

		/* The request is locked, so wait and then retry */
		spin_unlock(&inode->i_lock);
		error = nfs_wait_on_request(req);
		nfs_release_request(req);
		if (error != 0)
			goto out_err;
		spin_lock(&inode->i_lock);
	}

	/* Okay, the request matches. Update the region */
	if (offset < req->wb_offset) {
		req->wb_offset = offset;
		req->wb_pgbase = offset;
	}
	if (end > rqend)
		req->wb_bytes = end - req->wb_offset;
	else
		req->wb_bytes = rqend - req->wb_offset;
out_unlock:
	spin_unlock(&inode->i_lock);
	if (req)
		nfs_clear_request_commit(req);
	return req;
out_flushme:
	spin_unlock(&inode->i_lock);
	nfs_release_request(req);
	error = nfs_wb_page(inode, page);
out_err:
	return ERR_PTR(error);
}
