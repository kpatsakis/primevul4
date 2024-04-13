static int apparmor_inode_getattr(const struct path *path)
{
	return common_perm_path(OP_GETATTR, path, AA_MAY_META_READ);
}
