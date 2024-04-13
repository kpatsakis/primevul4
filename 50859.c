static int proc_pid_readlink(struct dentry * dentry, char __user * buffer, int buflen)
{
	int error = -EACCES;
	struct inode *inode = d_inode(dentry);
	struct path path;

	/* Are we allowed to snoop on the tasks file descriptors? */
	if (!proc_fd_access_allowed(inode))
		goto out;

	error = PROC_I(inode)->op.proc_get_link(dentry, &path);
	if (error)
		goto out;

	error = do_proc_readlink(&path, buffer, buflen);
	path_put(&path);
out:
	return error;
}
