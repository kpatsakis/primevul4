path_mountpoint(struct nameidata *nd, unsigned flags, struct path *path)
{
	const char *s = path_init(nd, flags);
	int err;
	if (IS_ERR(s))
		return PTR_ERR(s);
	while (!(err = link_path_walk(s, nd)) &&
		(err = mountpoint_last(nd)) > 0) {
		s = trailing_symlink(nd);
		if (IS_ERR(s)) {
			err = PTR_ERR(s);
			break;
		}
	}
	if (!err) {
		*path = nd->path;
		nd->path.mnt = NULL;
		nd->path.dentry = NULL;
		follow_mount(path);
	}
	terminate_walk(nd);
	return err;
}
