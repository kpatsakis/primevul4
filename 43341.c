path_mountpoint(int dfd, const char *name, struct path *path, unsigned int flags)
{
	struct file *base = NULL;
	struct nameidata nd;
	int err;

	err = path_init(dfd, name, flags | LOOKUP_PARENT, &nd, &base);
	if (unlikely(err))
		return err;

	current->total_link_count = 0;
	err = link_path_walk(name, &nd);
	if (err)
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
	if (base)
		fput(base);

	if (nd.root.mnt && !(nd.flags & LOOKUP_ROOT))
		path_put(&nd.root);

	return err;
}
