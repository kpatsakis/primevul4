mountpoint_last(struct nameidata *nd)
{
	int error = 0;
	struct dentry *dir = nd->path.dentry;
	struct path path;

	/* If we're in rcuwalk, drop out of it to handle last component */
	if (nd->flags & LOOKUP_RCU) {
		if (unlazy_walk(nd))
			return -ECHILD;
	}

	nd->flags &= ~LOOKUP_PARENT;

	if (unlikely(nd->last_type != LAST_NORM)) {
		error = handle_dots(nd, nd->last_type);
		if (error)
			return error;
		path.dentry = dget(nd->path.dentry);
	} else {
		path.dentry = d_lookup(dir, &nd->last);
		if (!path.dentry) {
			/*
			 * No cached dentry. Mounted dentries are pinned in the
			 * cache, so that means that this dentry is probably
			 * a symlink or the path doesn't actually point
			 * to a mounted dentry.
			 */
			path.dentry = lookup_slow(&nd->last, dir,
					     nd->flags | LOOKUP_NO_REVAL);
			if (IS_ERR(path.dentry))
				return PTR_ERR(path.dentry);
		}
	}
	if (d_is_negative(path.dentry)) {
		dput(path.dentry);
		return -ENOENT;
	}
	path.mnt = nd->path.mnt;
	return step_into(nd, &path, 0, d_backing_inode(path.dentry), 0);
}
