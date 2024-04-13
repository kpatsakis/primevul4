nfs_scan_commit(struct inode *inode, struct list_head *dst,
		struct nfs_commit_info *cinfo)
{
	int ret = 0;

	spin_lock(cinfo->lock);
	if (cinfo->mds->ncommit > 0) {
		const int max = INT_MAX;

		ret = nfs_scan_commit_list(&cinfo->mds->list, dst,
					   cinfo, max);
		ret += pnfs_scan_commit_lists(inode, cinfo, max - ret);
	}
	spin_unlock(cinfo->lock);
	return ret;
}
