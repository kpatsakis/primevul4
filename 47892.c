static int lxcfs_release(const char *path, struct fuse_file_info *fi)
{
	if (strncmp(path, "/cgroup", 7) == 0)
		return cg_release(path, fi);
	if (strncmp(path, "/proc", 5) == 0)
		return proc_release(path, fi);

	return -EINVAL;
}
