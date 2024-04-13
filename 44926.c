SYSCALL_DEFINE3(mkdirat, int, dfd, const char __user *, pathname, int, mode)
{
	int error = 0;
	char * tmp;
	struct dentry *dentry;
	struct nameidata nd;

	error = user_path_parent(dfd, pathname, &nd, &tmp);
	if (error)
		goto out_err;

	dentry = lookup_create(&nd, 1);
	error = PTR_ERR(dentry);
	if (IS_ERR(dentry))
		goto out_unlock;

	if (!IS_POSIXACL(nd.path.dentry->d_inode))
		mode &= ~current_umask();
	error = mnt_want_write(nd.path.mnt);
	if (error)
		goto out_dput;
	error = security_path_mkdir(&nd.path, dentry, mode);
	if (error)
		goto out_drop_write;
	error = vfs_mkdir(nd.path.dentry->d_inode, dentry, mode);
out_drop_write:
	mnt_drop_write(nd.path.mnt);
out_dput:
	dput(dentry);
out_unlock:
	mutex_unlock(&nd.path.dentry->d_inode->i_mutex);
	path_put(&nd.path);
	putname(tmp);
out_err:
	return error;
}
