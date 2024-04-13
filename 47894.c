int lxcfs_rmdir(const char *path)
{
	if (strncmp(path, "/cgroup", 7) == 0)
		return cg_rmdir(path);
	return -EINVAL;
}
