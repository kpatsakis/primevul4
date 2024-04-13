static int lxcfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset,
		struct fuse_file_info *fi)
{
	if (strcmp(path, "/") == 0) {
		if (filler(buf, "proc", NULL, 0) != 0 ||
				filler(buf, "cgroup", NULL, 0) != 0)
			return -EINVAL;
		return 0;
	}
	if (strncmp(path, "/cgroup", 7) == 0)
		return cg_readdir(path, buf, filler, offset, fi);
	if (strcmp(path, "/proc") == 0)
		return proc_readdir(path, buf, filler, offset, fi);
	return -EINVAL;
}
