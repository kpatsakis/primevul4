static int proc_read(const char *path, char *buf, size_t size, off_t offset,
		struct fuse_file_info *fi)
{
	struct file_info *f = (struct file_info *) fi->fh;

	switch (f->type) {
	case LXC_TYPE_PROC_MEMINFO:
		return proc_meminfo_read(buf, size, offset, fi);
	case LXC_TYPE_PROC_CPUINFO:
		return proc_cpuinfo_read(buf, size, offset, fi);
	case LXC_TYPE_PROC_UPTIME:
		return proc_uptime_read(buf, size, offset, fi);
	case LXC_TYPE_PROC_STAT:
		return proc_stat_read(buf, size, offset, fi);
	case LXC_TYPE_PROC_DISKSTATS:
		return proc_diskstats_read(buf, size, offset, fi);
	default:
		return -EINVAL;
	}
}
