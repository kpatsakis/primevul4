int vfs_rename(struct inode *old_dir, struct dentry *old_dentry,
	       struct inode *new_dir, struct dentry *new_dentry,
	       struct inode **delegated_inode, unsigned int flags)
{
	int error;
	bool is_dir = d_is_dir(old_dentry);
	const unsigned char *old_name;
	struct inode *source = old_dentry->d_inode;
	struct inode *target = new_dentry->d_inode;
	bool new_is_dir = false;
	unsigned max_links = new_dir->i_sb->s_max_links;

	if (source == target)
		return 0;

	error = may_delete(old_dir, old_dentry, is_dir);
	if (error)
		return error;

	if (!target) {
		error = may_create(new_dir, new_dentry);
	} else {
		new_is_dir = d_is_dir(new_dentry);

		if (!(flags & RENAME_EXCHANGE))
			error = may_delete(new_dir, new_dentry, is_dir);
		else
			error = may_delete(new_dir, new_dentry, new_is_dir);
	}
	if (error)
		return error;

	if (!old_dir->i_op->rename)
		return -EPERM;

	if (flags && !old_dir->i_op->rename2)
		return -EINVAL;

	/*
	 * If we are going to change the parent - check write permissions,
	 * we'll need to flip '..'.
	 */
	if (new_dir != old_dir) {
		if (is_dir) {
			error = inode_permission(source, MAY_WRITE);
			if (error)
				return error;
		}
		if ((flags & RENAME_EXCHANGE) && new_is_dir) {
			error = inode_permission(target, MAY_WRITE);
			if (error)
				return error;
		}
	}

	error = security_inode_rename(old_dir, old_dentry, new_dir, new_dentry,
				      flags);
	if (error)
		return error;

	old_name = fsnotify_oldname_init(old_dentry->d_name.name);
	dget(new_dentry);
	if (!is_dir || (flags & RENAME_EXCHANGE))
		lock_two_nondirectories(source, target);
	else if (target)
		mutex_lock(&target->i_mutex);

	error = -EBUSY;
	if (d_mountpoint(old_dentry) || d_mountpoint(new_dentry))
		goto out;

	if (max_links && new_dir != old_dir) {
		error = -EMLINK;
		if (is_dir && !new_is_dir && new_dir->i_nlink >= max_links)
			goto out;
		if ((flags & RENAME_EXCHANGE) && !is_dir && new_is_dir &&
		    old_dir->i_nlink >= max_links)
			goto out;
	}
	if (is_dir && !(flags & RENAME_EXCHANGE) && target)
		shrink_dcache_parent(new_dentry);
	if (!is_dir) {
		error = try_break_deleg(source, delegated_inode);
		if (error)
			goto out;
	}
	if (target && !new_is_dir) {
		error = try_break_deleg(target, delegated_inode);
		if (error)
			goto out;
	}
	if (!flags) {
		error = old_dir->i_op->rename(old_dir, old_dentry,
					      new_dir, new_dentry);
	} else {
		error = old_dir->i_op->rename2(old_dir, old_dentry,
					       new_dir, new_dentry, flags);
	}
	if (error)
		goto out;

	if (!(flags & RENAME_EXCHANGE) && target) {
		if (is_dir)
			target->i_flags |= S_DEAD;
		dont_mount(new_dentry);
	}
	if (!(old_dir->i_sb->s_type->fs_flags & FS_RENAME_DOES_D_MOVE)) {
		if (!(flags & RENAME_EXCHANGE))
			d_move(old_dentry, new_dentry);
		else
			d_exchange(old_dentry, new_dentry);
	}
out:
	if (!is_dir || (flags & RENAME_EXCHANGE))
		unlock_two_nondirectories(source, target);
	else if (target)
		mutex_unlock(&target->i_mutex);
	dput(new_dentry);
	if (!error) {
		fsnotify_move(old_dir, new_dir, old_name, is_dir,
			      !(flags & RENAME_EXCHANGE) ? target : NULL, old_dentry);
		if (flags & RENAME_EXCHANGE) {
			fsnotify_move(new_dir, old_dir, old_dentry->d_name.name,
				      new_is_dir, NULL, new_dentry);
		}
	}
	fsnotify_oldname_free(old_name);

	return error;
}
