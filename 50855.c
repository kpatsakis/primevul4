static const char *proc_pid_get_link(struct dentry *dentry,
				     struct inode *inode,
				     struct delayed_call *done)
{
	struct path path;
	int error = -EACCES;

	if (!dentry)
		return ERR_PTR(-ECHILD);

	/* Are we allowed to snoop on the tasks file descriptors? */
	if (!proc_fd_access_allowed(inode))
		goto out;

	error = PROC_I(inode)->op.proc_get_link(dentry, &path);
	if (error)
		goto out;

	nd_jump_link(&path);
	return NULL;
out:
	return ERR_PTR(error);
}
