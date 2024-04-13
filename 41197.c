static bool mnt_ns_loop(struct path *path)
{
	/* Could bind mounting the mount namespace inode cause a
	 * mount namespace loop?
	 */
	struct inode *inode = path->dentry->d_inode;
	struct proc_inode *ei;
	struct mnt_namespace *mnt_ns;

	if (!proc_ns_inode(inode))
		return false;

	ei = PROC_I(inode);
	if (ei->ns_ops != &mntns_operations)
		return false;

	mnt_ns = ei->ns;
	return current->nsproxy->mnt_ns->seq >= mnt_ns->seq;
}
