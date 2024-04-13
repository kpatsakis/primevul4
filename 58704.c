static int do_tmpfile(struct nameidata *nd, unsigned flags,
		const struct open_flags *op,
		struct file *file, int *opened)
{
	struct dentry *child;
	struct path path;
	int error = path_lookupat(nd, flags | LOOKUP_DIRECTORY, &path);
	if (unlikely(error))
		return error;
	error = mnt_want_write(path.mnt);
	if (unlikely(error))
		goto out;
	child = vfs_tmpfile(path.dentry, op->mode, op->open_flag);
	error = PTR_ERR(child);
	if (unlikely(IS_ERR(child)))
		goto out2;
	dput(path.dentry);
	path.dentry = child;
	audit_inode(nd->name, child, 0);
	/* Don't check for other permissions, the inode was just created */
	error = may_open(&path, 0, op->open_flag);
	if (error)
		goto out2;
	file->f_path.mnt = path.mnt;
	error = finish_open(file, child, NULL, opened);
	if (error)
		goto out2;
	error = open_check_o_direct(file);
	if (error)
		fput(file);
out2:
	mnt_drop_write(path.mnt);
out:
	path_put(&path);
	return error;
}
