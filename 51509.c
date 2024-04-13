static int apparmor_path_truncate(const struct path *path)
{
	return common_perm_path(OP_TRUNC, path, MAY_WRITE | AA_MAY_META_WRITE);
}
