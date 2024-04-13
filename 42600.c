static int apparmor_path_mknod(struct path *dir, struct dentry *dentry,
			       int mode, unsigned int dev)
{
	return common_perm_create(OP_MKNOD, dir, dentry, AA_MAY_CREATE, mode);
}
