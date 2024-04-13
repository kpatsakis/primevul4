static int do_mq_open(const char __user *u_name, int oflag, umode_t mode,
		      struct mq_attr *attr)
{
	struct path path;
	struct file *filp;
	struct filename *name;
	int fd, error;
	struct ipc_namespace *ipc_ns = current->nsproxy->ipc_ns;
	struct vfsmount *mnt = ipc_ns->mq_mnt;
	struct dentry *root = mnt->mnt_root;
	int ro;

	audit_mq_open(oflag, mode, attr);

	if (IS_ERR(name = getname(u_name)))
		return PTR_ERR(name);

	fd = get_unused_fd_flags(O_CLOEXEC);
	if (fd < 0)
		goto out_putname;

	ro = mnt_want_write(mnt);	/* we'll drop it in any case */
	error = 0;
	inode_lock(d_inode(root));
	path.dentry = lookup_one_len(name->name, root, strlen(name->name));
	if (IS_ERR(path.dentry)) {
		error = PTR_ERR(path.dentry);
		goto out_putfd;
	}
	path.mnt = mntget(mnt);

	if (oflag & O_CREAT) {
		if (d_really_is_positive(path.dentry)) {	/* entry already exists */
			audit_inode(name, path.dentry, 0);
			if (oflag & O_EXCL) {
				error = -EEXIST;
				goto out;
			}
			filp = do_open(&path, oflag);
		} else {
			if (ro) {
				error = ro;
				goto out;
			}
			audit_inode_parent_hidden(name, root);
			filp = do_create(ipc_ns, d_inode(root), &path,
					 oflag, mode, attr);
		}
	} else {
		if (d_really_is_negative(path.dentry)) {
			error = -ENOENT;
			goto out;
		}
		audit_inode(name, path.dentry, 0);
		filp = do_open(&path, oflag);
	}

	if (!IS_ERR(filp))
		fd_install(fd, filp);
	else
		error = PTR_ERR(filp);
out:
	path_put(&path);
out_putfd:
	if (error) {
		put_unused_fd(fd);
		fd = error;
	}
	inode_unlock(d_inode(root));
	if (!ro)
		mnt_drop_write(mnt);
out_putname:
	putname(name);
	return fd;
}
