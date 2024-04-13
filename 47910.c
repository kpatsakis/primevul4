static int proc_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset,
		struct fuse_file_info *fi)
{
	if (filler(buf, "cpuinfo", NULL, 0) != 0 ||
				filler(buf, "meminfo", NULL, 0) != 0 ||
				filler(buf, "stat", NULL, 0) != 0 ||
				filler(buf, "uptime", NULL, 0) != 0 ||
				filler(buf, "diskstats", NULL, 0) != 0)
		return -EINVAL;
	return 0;
}
