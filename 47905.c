static int proc_cpuinfo_read(char *buf, size_t size, off_t offset,
		struct fuse_file_info *fi)
{
	struct fuse_context *fc = fuse_get_context();
	struct file_info *d = (struct file_info *)fi->fh;
	char *cg;
	char *cpuset = NULL;
	char *line = NULL;
	size_t linelen = 0, total_len = 0, rv = 0;
	bool am_printing = false;
	int curcpu = -1;
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

	cg = get_pid_cgroup(fc->pid, "cpuset");
	if (!cg)
		return read_file("proc/cpuinfo", buf, size, d);

	cpuset = get_cpuset(cg);
	if (!cpuset)
		goto err;

	f = fopen("/proc/cpuinfo", "r");
	if (!f)
		goto err;

	while (getline(&line, &linelen, f) != -1) {
		size_t l;
		if (is_processor_line(line)) {
			am_printing = cpuline_in_cpuset(line, cpuset);
			if (am_printing) {
				curcpu ++;
				l = snprintf(cache, cache_size, "processor	: %d\n", curcpu);
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
				if (l < cache_size){
					cache += l;
					cache_size -= l;
					total_len += l;
				}else{
					cache += cache_size;
					total_len += cache_size;
					cache_size = 0;
					break;
				}
			}
			continue;
		}
		if (am_printing) {
			l = snprintf(cache, cache_size, "%s", line);
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
			if (l < cache_size) {
				cache += l;
				cache_size -= l;
				total_len += l;
			} else {
				cache += cache_size;
				total_len += cache_size;
				cache_size = 0;
				break;
			}
		}
	}

	d->cached = 1;
	d->size = total_len;
	if (total_len > size ) total_len = size;

	/* read from off 0 */
	memcpy(buf, d->buf, total_len);
	rv = total_len;
err:
	if (f)
		fclose(f);
	free(line);
	free(cpuset);
	free(cg);
	return rv;
}
