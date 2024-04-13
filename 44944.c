static int do_path_lookup(int dfd, const char *name,
				unsigned int flags, struct nameidata *nd)
{
	int retval = path_init(dfd, name, flags, nd);
	if (!retval)
		retval = path_walk(name, nd);
	if (unlikely(!retval && !audit_dummy_context() && nd->path.dentry &&
				nd->path.dentry->d_inode))
		audit_inode(name, nd->path.dentry);
	if (nd->root.mnt) {
		path_put(&nd->root);
		nd->root.mnt = NULL;
	}
	return retval;
}
