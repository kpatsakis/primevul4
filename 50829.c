int pid_delete_dentry(const struct dentry *dentry)
{
	/* Is the task we represent dead?
	 * If so, then don't put the dentry on the lru list,
	 * kill it immediately.
	 */
	return proc_inode_is_dead(d_inode(dentry));
}
