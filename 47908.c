static int proc_open(const char *path, struct fuse_file_info *fi)
{
	int type = -1;
	struct file_info *info;

	if (strcmp(path, "/proc/meminfo") == 0)
		type = LXC_TYPE_PROC_MEMINFO;
	else if (strcmp(path, "/proc/cpuinfo") == 0)
		type = LXC_TYPE_PROC_CPUINFO;
	else if (strcmp(path, "/proc/uptime") == 0)
		type = LXC_TYPE_PROC_UPTIME;
	else if (strcmp(path, "/proc/stat") == 0)
		type = LXC_TYPE_PROC_STAT;
	else if (strcmp(path, "/proc/diskstats") == 0)
		type = LXC_TYPE_PROC_DISKSTATS;
	if (type == -1)
		return -ENOENT;

	info = malloc(sizeof(*info));
	if (!info)
		return -ENOMEM;

	memset(info, 0, sizeof(*info));
	info->type = type;

	info->buflen = get_procfile_size(path) + BUF_RESERVE_SIZE;
	do {
		info->buf = malloc(info->buflen);
	} while (!info->buf);
	memset(info->buf, 0, info->buflen);
	/* set actual size to buffer size */
	info->size = info->buflen;

	fi->fh = (unsigned long)info;
	return 0;
}
