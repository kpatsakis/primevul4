nfs_clear_request_commit(struct nfs_page *req)
{
	if (test_bit(PG_CLEAN, &req->wb_flags)) {
		struct inode *inode = req->wb_context->dentry->d_inode;
		struct nfs_commit_info cinfo;

		nfs_init_cinfo_from_inode(&cinfo, inode);
		if (!pnfs_clear_request_commit(req, &cinfo)) {
			spin_lock(cinfo.lock);
			nfs_request_remove_commit_list(req, &cinfo);
			spin_unlock(cinfo.lock);
		}
		nfs_clear_page_commit(req->wb_page);
	}
}
