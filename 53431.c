int ovl_copy_up(struct dentry *dentry)
{
	int err;

	err = 0;
	while (!err) {
		struct dentry *next;
		struct dentry *parent;
		struct path lowerpath;
		struct kstat stat;
		enum ovl_path_type type = ovl_path_type(dentry);

		if (OVL_TYPE_UPPER(type))
			break;

		next = dget(dentry);
		/* find the topmost dentry not yet copied up */
		for (;;) {
			parent = dget_parent(next);

			type = ovl_path_type(parent);
			if (OVL_TYPE_UPPER(type))
				break;

			dput(next);
			next = parent;
		}

		ovl_path_lower(next, &lowerpath);
		err = vfs_getattr(&lowerpath, &stat);
		if (!err)
			err = ovl_copy_up_one(parent, next, &lowerpath, &stat, NULL);

		dput(parent);
		dput(next);
	}

	return err;
}
