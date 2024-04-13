static int apparmor_path_chmod(const struct path *path, umode_t mode)
{
	return common_perm_path(OP_CHMOD, path, AA_MAY_CHMOD);
}
