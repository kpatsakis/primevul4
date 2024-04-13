static int lxcfs_read(const char *path, char *buf, size_t size, off_t offset,
		struct fuse_file_info *fi)
{
	if (strncmp(path, "/cgroup", 7) == 0)
		return cg_read(path, buf, size, offset, fi);
	if (strncmp(path, "/proc", 5) == 0)
		return proc_read(path, buf, size, offset, fi);

	return -EINVAL;
}
