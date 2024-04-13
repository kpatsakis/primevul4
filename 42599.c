static int apparmor_path_mkdir(struct path *dir, struct dentry *dentry,
			       int mode)
{
	return common_perm_create(OP_MKDIR, dir, dentry, AA_MAY_CREATE,
				  S_IFDIR);
}
