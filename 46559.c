path_mountpoint(int dfd, const struct filename *name, struct path *path,
		unsigned int flags)
{
	struct nameidata nd;
	int err;

	err = path_init(dfd, name, flags, &nd);
	if (unlikely(err))
		goto out;

	err = mountpoint_last(&nd, path);
	while (err > 0) {
		void *cookie;
		struct path link = *path;
		err = may_follow_link(&link, &nd);
		if (unlikely(err))
			break;
		nd.flags |= LOOKUP_PARENT;
		err = follow_link(&link, &nd, &cookie);
		if (err)
			break;
		err = mountpoint_last(&nd, path);
		put_link(&nd, &link, cookie);
	}
out:
	path_cleanup(&nd);
	return err;
}
