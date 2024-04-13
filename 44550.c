static void nfs_inode_remove_request(struct nfs_page *req)
{
	struct inode *inode = req->wb_context->dentry->d_inode;
	struct nfs_inode *nfsi = NFS_I(inode);

	spin_lock(&inode->i_lock);
	if (likely(!PageSwapCache(req->wb_page))) {
		set_page_private(req->wb_page, 0);
		ClearPagePrivate(req->wb_page);
		clear_bit(PG_MAPPED, &req->wb_flags);
	}
	nfsi->npages--;
	spin_unlock(&inode->i_lock);
	nfs_release_request(req);
}
