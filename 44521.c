static void nfs_commit_release_pages(struct nfs_commit_data *data)
{
	struct nfs_page	*req;
	int status = data->task.tk_status;
	struct nfs_commit_info cinfo;

	while (!list_empty(&data->pages)) {
		req = nfs_list_entry(data->pages.next);
		nfs_list_remove_request(req);
		nfs_clear_page_commit(req->wb_page);

		dprintk("NFS:       commit (%s/%llu %d@%lld)",
			req->wb_context->dentry->d_sb->s_id,
			(unsigned long long)NFS_FILEID(req->wb_context->dentry->d_inode),
			req->wb_bytes,
			(long long)req_offset(req));
		if (status < 0) {
			nfs_context_set_write_error(req->wb_context, status);
			nfs_inode_remove_request(req);
			dprintk(", error = %d\n", status);
			goto next;
		}

		/* Okay, COMMIT succeeded, apparently. Check the verifier
		 * returned by the server against all stored verfs. */
		if (!memcmp(&req->wb_verf, &data->verf.verifier, sizeof(req->wb_verf))) {
			/* We have a match */
			nfs_inode_remove_request(req);
			dprintk(" OK\n");
			goto next;
		}
		/* We have a mismatch. Write the page again */
		dprintk(" mismatch\n");
		nfs_mark_request_dirty(req);
		set_bit(NFS_CONTEXT_RESEND_WRITES, &req->wb_context->flags);
	next:
		nfs_unlock_and_release_request(req);
	}
	nfs_init_cinfo(&cinfo, data->inode, data->dreq);
	if (atomic_dec_and_test(&cinfo.mds->rpcs_out))
		nfs_commit_clear_lock(NFS_I(data->inode));
}
