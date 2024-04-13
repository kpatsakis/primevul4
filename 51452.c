static int do_tmpfile(struct nameidata *nd, unsigned flags,
		const struct open_flags *op,
		struct file *file, int *opened)
{
	static const struct qstr name = QSTR_INIT("/", 1);
	struct dentry *child;
	struct inode *dir;
	struct path path;
	int error = path_lookupat(nd, flags | LOOKUP_DIRECTORY, &path);
	if (unlikely(error))
		return error;
	error = mnt_want_write(path.mnt);
	if (unlikely(error))
		goto out;
	dir = path.dentry->d_inode;
	/* we want directory to be writable */
	error = inode_permission(dir, MAY_WRITE | MAY_EXEC);
	if (error)
		goto out2;
	if (!dir->i_op->tmpfile) {
		error = -EOPNOTSUPP;
		goto out2;
	}
	child = d_alloc(path.dentry, &name);
	if (unlikely(!child)) {
		error = -ENOMEM;
		goto out2;
	}
	dput(path.dentry);
	path.dentry = child;
	error = dir->i_op->tmpfile(dir, child, op->mode);
	if (error)
		goto out2;
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
	if (error) {
		fput(file);
	} else if (!(op->open_flag & O_EXCL)) {
		struct inode *inode = file_inode(file);
		spin_lock(&inode->i_lock);
		inode->i_state |= I_LINKABLE;
		spin_unlock(&inode->i_lock);
	}
out2:
	mnt_drop_write(path.mnt);
out:
	path_put(&path);
	return error;
}
