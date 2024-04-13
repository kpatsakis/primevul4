static void nfs_inode_add_request(struct inode *inode, struct nfs_page *req)
{
	struct nfs_inode *nfsi = NFS_I(inode);

	/* Lock the request! */
	nfs_lock_request(req);

	spin_lock(&inode->i_lock);
	if (!nfsi->npages && NFS_PROTO(inode)->have_delegation(inode, FMODE_WRITE))
		inode->i_version++;
	/*
	 * Swap-space should not get truncated. Hence no need to plug the race
	 * with invalidate/truncate.
	 */
	if (likely(!PageSwapCache(req->wb_page))) {
		set_bit(PG_MAPPED, &req->wb_flags);
		SetPagePrivate(req->wb_page);
		set_page_private(req->wb_page, (unsigned long)req);
	}
	nfsi->npages++;
	kref_get(&req->wb_kref);
	spin_unlock(&inode->i_lock);
}
