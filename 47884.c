int lxcfs_chown(const char *path, uid_t uid, gid_t gid)
{
	if (strncmp(path, "/cgroup", 7) == 0)
		return cg_chown(path, uid, gid);

	return -EINVAL;
}
