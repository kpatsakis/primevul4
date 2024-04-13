int vfs_path_lookup(struct dentry *dentry, struct vfsmount *mnt,
		    const char *name, unsigned int flags,
		    struct path *path)
{
	struct filename *filename = getname_kernel(name);
	int err = PTR_ERR(filename);

	BUG_ON(flags & LOOKUP_PARENT);

	/* the first argument of filename_lookup() is ignored with LOOKUP_ROOT */
	if (!IS_ERR(filename)) {
		struct nameidata nd;
		nd.root.dentry = dentry;
		nd.root.mnt = mnt;
		err = filename_lookup(AT_FDCWD, filename,
				      flags | LOOKUP_ROOT, &nd);
		if (!err)
			*path = nd.path;
		putname(filename);
	}
	return err;
}
