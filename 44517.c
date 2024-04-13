int nfs_commit_inode(struct inode *inode, int how)
{
	LIST_HEAD(head);
	struct nfs_commit_info cinfo;
	int may_wait = how & FLUSH_SYNC;
	int res;

	res = nfs_commit_set_lock(NFS_I(inode), may_wait);
	if (res <= 0)
		goto out_mark_dirty;
	nfs_init_cinfo_from_inode(&cinfo, inode);
	res = nfs_scan_commit(inode, &head, &cinfo);
	if (res) {
		int error;

		error = nfs_generic_commit_list(inode, &head, how, &cinfo);
		if (error < 0)
			return error;
		if (!may_wait)
			goto out_mark_dirty;
		error = wait_on_bit(&NFS_I(inode)->flags,
				NFS_INO_COMMIT,
				nfs_wait_bit_killable,
				TASK_KILLABLE);
		if (error < 0)
			return error;
	} else
		nfs_commit_clear_lock(NFS_I(inode));
	return res;
	/* Note: If we exit without ensuring that the commit is complete,
	 * we must mark the inode as dirty. Otherwise, future calls to
	 * sync_inode() with the WB_SYNC_ALL flag set will fail to ensure
	 * that the data is on the disk.
	 */
out_mark_dirty:
	__mark_inode_dirty(inode, I_DIRTY_DATASYNC);
	return res;
}
