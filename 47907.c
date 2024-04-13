static int proc_meminfo_read(char *buf, size_t size, off_t offset,
		struct fuse_file_info *fi)
{
	struct fuse_context *fc = fuse_get_context();
	struct file_info *d = (struct file_info *)fi->fh;
	char *cg;
	char *memusage_str = NULL, *memstat_str = NULL,
		*memswlimit_str = NULL, *memswusage_str = NULL;
	unsigned long memlimit = 0, memusage = 0, memswlimit = 0, memswusage = 0,
		cached = 0, hosttotal = 0;
	char *line = NULL;
	size_t linelen = 0, total_len = 0, rv = 0;
	char *cache = d->buf;
	size_t cache_size = d->buflen;
	FILE *f = NULL;

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

	cg = get_pid_cgroup(fc->pid, "memory");
	if (!cg)
		return read_file("/proc/meminfo", buf, size, d);

	memlimit = get_min_memlimit(cg);
	if (!cgfs_get_value("memory", cg, "memory.usage_in_bytes", &memusage_str))
		goto err;
	if (!cgfs_get_value("memory", cg, "memory.stat", &memstat_str))
		goto err;

	if(cgfs_get_value("memory", cg, "memory.memsw.limit_in_bytes", &memswlimit_str) &&
		cgfs_get_value("memory", cg, "memory.memsw.usage_in_bytes", &memswusage_str))
	{
		memswlimit = strtoul(memswlimit_str, NULL, 10);
		memswusage = strtoul(memswusage_str, NULL, 10);
		memswlimit /= 1024;
		memswusage /= 1024;
	}
	
	memusage = strtoul(memusage_str, NULL, 10);
	memlimit /= 1024;
	memusage /= 1024;
	get_mem_cached(memstat_str, &cached);

	f = fopen("/proc/meminfo", "r");
	if (!f)
		goto err;

	while (getline(&line, &linelen, f) != -1) {
		size_t l;
		char *printme, lbuf[100];

		memset(lbuf, 0, 100);
		if (startswith(line, "MemTotal:")) {
			sscanf(line+14, "%lu", &hosttotal);
			if (hosttotal < memlimit)
				memlimit = hosttotal;
			snprintf(lbuf, 100, "MemTotal:       %8lu kB\n", memlimit);
			printme = lbuf;
		} else if (startswith(line, "MemFree:")) {
			snprintf(lbuf, 100, "MemFree:        %8lu kB\n", memlimit - memusage);
			printme = lbuf;
		} else if (startswith(line, "MemAvailable:")) {
			snprintf(lbuf, 100, "MemAvailable:   %8lu kB\n", memlimit - memusage);
			printme = lbuf;
		} else if (startswith(line, "SwapTotal:") && memswlimit > 0) {
			snprintf(lbuf, 100, "SwapTotal:      %8lu kB\n", memswlimit - memlimit);
			printme = lbuf;
		} else if (startswith(line, "SwapFree:") && memswlimit > 0 && memswusage > 0) {
			snprintf(lbuf, 100, "SwapFree:       %8lu kB\n", 
				(memswlimit - memlimit) - (memswusage - memusage));
			printme = lbuf;
		} else if (startswith(line, "Buffers:")) {
			snprintf(lbuf, 100, "Buffers:        %8lu kB\n", 0UL);
			printme = lbuf;
		} else if (startswith(line, "Cached:")) {
			snprintf(lbuf, 100, "Cached:         %8lu kB\n", cached);
			printme = lbuf;
		} else if (startswith(line, "SwapCached:")) {
			snprintf(lbuf, 100, "SwapCached:     %8lu kB\n", 0UL);
			printme = lbuf;
		} else
			printme = line;

		l = snprintf(cache, cache_size, "%s", printme);
		if (l < 0) {
			perror("Error writing to cache");
			rv = 0;
			goto err;

		}
		if (l >= cache_size) {
			fprintf(stderr, "Internal error: truncated write to cache\n");
			rv = 0;
			goto err;
		}

		cache += l;
		cache_size -= l;
		total_len += l;
	}

	d->cached = 1;
	d->size = total_len;
	if (total_len > size ) total_len = size;
	memcpy(buf, d->buf, total_len);

	rv = total_len;
err:
	if (f)
		fclose(f);
	free(line);
	free(cg);
	free(memusage_str);
	free(memswlimit_str);
	free(memswusage_str);
	free(memstat_str);
	return rv;
}
