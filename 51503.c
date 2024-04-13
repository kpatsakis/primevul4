static int apparmor_path_chown(const struct path *path, kuid_t uid, kgid_t gid)
{
	return common_perm_path(OP_CHOWN, path, AA_MAY_CHOWN);
}
