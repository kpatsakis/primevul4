static struct nfs_page *nfs_find_and_lock_request(struct page *page, bool nonblock)
{
	struct inode *inode = page_file_mapping(page)->host;
	struct nfs_page *req;
	int ret;

	spin_lock(&inode->i_lock);
	for (;;) {
		req = nfs_page_find_request_locked(NFS_I(inode), page);
		if (req == NULL)
			break;
		if (nfs_lock_request(req))
			break;
		/* Note: If we hold the page lock, as is the case in nfs_writepage,
		 *	 then the call to nfs_lock_request() will always
		 *	 succeed provided that someone hasn't already marked the
		 *	 request as dirty (in which case we don't care).
		 */
		spin_unlock(&inode->i_lock);
		if (!nonblock)
			ret = nfs_wait_on_request(req);
		else
			ret = -EAGAIN;
		nfs_release_request(req);
		if (ret != 0)
			return ERR_PTR(ret);
		spin_lock(&inode->i_lock);
	}
	spin_unlock(&inode->i_lock);
	return req;
}
