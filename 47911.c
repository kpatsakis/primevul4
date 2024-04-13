static int proc_uptime_read(char *buf, size_t size, off_t offset,
		struct fuse_file_info *fi)
{
	struct fuse_context *fc = fuse_get_context();
	struct file_info *d = (struct file_info *)fi->fh;
	long int reaperage = getreaperage(fc->pid);;
	unsigned long int busytime = get_reaper_busy(fc->pid), idletime;
	char *cache = d->buf;
	size_t total_len = 0;

	if (offset){
		if (offset > d->size)
			return -EINVAL;
		if (!d->cached)
			return 0;
		int left = d->size - offset;
		total_len = left > size ? size: left;
		memcpy(buf, cache + offset, total_len);
		return total_len;
	}

	idletime = reaperage - busytime;
	if (idletime > reaperage)
		idletime = reaperage;

	total_len = snprintf(d->buf, d->size, "%ld.0 %lu.0\n", reaperage, idletime);
	if (total_len < 0){
		perror("Error writing to cache");
		return 0;
	}

	d->size = (int)total_len;
	d->cached = 1;

	if (total_len > size) total_len = size;

	memcpy(buf, d->buf, total_len);
	return total_len;
}
