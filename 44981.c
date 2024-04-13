int vfs_path_lookup(struct dentry *dentry, struct vfsmount *mnt,
		    const char *name, unsigned int flags,
		    struct nameidata *nd)
{
	int retval;

	/* same as do_path_lookup */
	nd->last_type = LAST_ROOT;
	nd->flags = flags;
	nd->depth = 0;

	nd->path.dentry = dentry;
	nd->path.mnt = mnt;
	path_get(&nd->path);
	nd->root = nd->path;
	path_get(&nd->root);

	retval = path_walk(name, nd);
	if (unlikely(!retval && !audit_dummy_context() && nd->path.dentry &&
				nd->path.dentry->d_inode))
		audit_inode(name, nd->path.dentry);

	path_put(&nd->root);
	nd->root.mnt = NULL;

	return retval;
}
