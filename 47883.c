int lxcfs_chmod(const char *path, mode_t mode)
{
	if (strncmp(path, "/cgroup", 7) == 0)
		return cg_chmod(path, mode);
	return -EINVAL;
}
