static int lxcfs_open(const char *path, struct fuse_file_info *fi)
{
	if (strncmp(path, "/cgroup", 7) == 0)
		return cg_open(path, fi);
	if (strncmp(path, "/proc", 5) == 0)
		return proc_open(path, fi);

	return -EINVAL;
}
