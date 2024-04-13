int lxcfs_write(const char *path, const char *buf, size_t size, off_t offset,
	     struct fuse_file_info *fi)
{
	if (strncmp(path, "/cgroup", 7) == 0) {
		return cg_write(path, buf, size, offset, fi);
	}

	return -EINVAL;
}
