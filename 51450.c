static int do_last(struct nameidata *nd,
		   struct file *file, const struct open_flags *op,
		   int *opened)
{
	struct dentry *dir = nd->path.dentry;
	int open_flag = op->open_flag;
	bool will_truncate = (open_flag & O_TRUNC) != 0;
	bool got_write = false;
	int acc_mode = op->acc_mode;
	unsigned seq;
	struct inode *inode;
	struct path save_parent = { .dentry = NULL, .mnt = NULL };
	struct path path;
	bool retried = false;
	int error;

	nd->flags &= ~LOOKUP_PARENT;
	nd->flags |= op->intent;

	if (nd->last_type != LAST_NORM) {
		error = handle_dots(nd, nd->last_type);
		if (unlikely(error))
			return error;
		goto finish_open;
	}

	if (!(open_flag & O_CREAT)) {
		if (nd->last.name[nd->last.len])
			nd->flags |= LOOKUP_FOLLOW | LOOKUP_DIRECTORY;
		/* we _can_ be in RCU mode here */
		error = lookup_fast(nd, &path, &inode, &seq);
		if (likely(error > 0))
			goto finish_lookup;

		if (error < 0)
			return error;

		BUG_ON(nd->inode != dir->d_inode);
		BUG_ON(nd->flags & LOOKUP_RCU);
	} else {
		/* create side of things */
		/*
		 * This will *only* deal with leaving RCU mode - LOOKUP_JUMPED
		 * has been cleared when we got to the last component we are
		 * about to look up
		 */
		error = complete_walk(nd);
		if (error)
			return error;

		audit_inode(nd->name, dir, LOOKUP_PARENT);
		/* trailing slashes? */
		if (unlikely(nd->last.name[nd->last.len]))
			return -EISDIR;
	}

retry_lookup:
	if (op->open_flag & (O_CREAT | O_TRUNC | O_WRONLY | O_RDWR)) {
		error = mnt_want_write(nd->path.mnt);
		if (!error)
			got_write = true;
		/*
		 * do _not_ fail yet - we might not need that or fail with
		 * a different error; let lookup_open() decide; we'll be
		 * dropping this one anyway.
		 */
	}
	inode_lock(dir->d_inode);
	error = lookup_open(nd, &path, file, op, got_write, opened);
	inode_unlock(dir->d_inode);

	if (error <= 0) {
		if (error)
			goto out;

		if ((*opened & FILE_CREATED) ||
		    !S_ISREG(file_inode(file)->i_mode))
			will_truncate = false;

		audit_inode(nd->name, file->f_path.dentry, 0);
		goto opened;
	}

	if (*opened & FILE_CREATED) {
		/* Don't check for write permission, don't truncate */
		open_flag &= ~O_TRUNC;
		will_truncate = false;
		acc_mode = 0;
		path_to_nameidata(&path, nd);
		goto finish_open_created;
	}

	/*
	 * If atomic_open() acquired write access it is dropped now due to
	 * possible mount and symlink following (this might be optimized away if
	 * necessary...)
	 */
	if (got_write) {
		mnt_drop_write(nd->path.mnt);
		got_write = false;
	}

	if (unlikely(d_is_negative(path.dentry))) {
		path_to_nameidata(&path, nd);
		return -ENOENT;
	}

	/*
	 * create/update audit record if it already exists.
	 */
	audit_inode(nd->name, path.dentry, 0);

	if (unlikely((open_flag & (O_EXCL | O_CREAT)) == (O_EXCL | O_CREAT))) {
		path_to_nameidata(&path, nd);
		return -EEXIST;
	}

	error = follow_managed(&path, nd);
	if (unlikely(error < 0))
		return error;

	seq = 0;	/* out of RCU mode, so the value doesn't matter */
	inode = d_backing_inode(path.dentry);
finish_lookup:
	if (nd->depth)
		put_link(nd);
	error = should_follow_link(nd, &path, nd->flags & LOOKUP_FOLLOW,
				   inode, seq);
	if (unlikely(error))
		return error;

	if ((nd->flags & LOOKUP_RCU) || nd->path.mnt != path.mnt) {
		path_to_nameidata(&path, nd);
	} else {
		save_parent.dentry = nd->path.dentry;
		save_parent.mnt = mntget(path.mnt);
		nd->path.dentry = path.dentry;

	}
	nd->inode = inode;
	nd->seq = seq;
	/* Why this, you ask?  _Now_ we might have grown LOOKUP_JUMPED... */
finish_open:
	error = complete_walk(nd);
	if (error) {
		path_put(&save_parent);
		return error;
	}
	audit_inode(nd->name, nd->path.dentry, 0);
	if (unlikely(d_is_symlink(nd->path.dentry)) && !(open_flag & O_PATH)) {
		error = -ELOOP;
		goto out;
	}
	error = -EISDIR;
	if ((open_flag & O_CREAT) && d_is_dir(nd->path.dentry))
		goto out;
	error = -ENOTDIR;
	if ((nd->flags & LOOKUP_DIRECTORY) && !d_can_lookup(nd->path.dentry))
		goto out;
	if (!d_is_reg(nd->path.dentry))
		will_truncate = false;

	if (will_truncate) {
		error = mnt_want_write(nd->path.mnt);
		if (error)
			goto out;
		got_write = true;
	}
finish_open_created:
	if (likely(!(open_flag & O_PATH))) {
		error = may_open(&nd->path, acc_mode, open_flag);
		if (error)
			goto out;
	}
	BUG_ON(*opened & FILE_OPENED); /* once it's opened, it's opened */
	error = vfs_open(&nd->path, file, current_cred());
	if (!error) {
		*opened |= FILE_OPENED;
	} else {
		if (error == -EOPENSTALE)
			goto stale_open;
		goto out;
	}
opened:
	error = open_check_o_direct(file);
	if (error)
		goto exit_fput;
	error = ima_file_check(file, op->acc_mode, *opened);
	if (error)
		goto exit_fput;

	if (will_truncate) {
		error = handle_truncate(file);
		if (error)
			goto exit_fput;
	}
out:
	if (unlikely(error > 0)) {
		WARN_ON(1);
		error = -EINVAL;
	}
	if (got_write)
		mnt_drop_write(nd->path.mnt);
	path_put(&save_parent);
	return error;

exit_fput:
	fput(file);
	goto out;

stale_open:
	/* If no saved parent or already retried then can't retry */
	if (!save_parent.dentry || retried)
		goto out;

	BUG_ON(save_parent.dentry != dir);
	path_put(&nd->path);
	nd->path = save_parent;
	nd->inode = dir->d_inode;
	save_parent.mnt = NULL;
	save_parent.dentry = NULL;
	if (got_write) {
		mnt_drop_write(nd->path.mnt);
		got_write = false;
	}
	retried = true;
	goto retry_lookup;
}
