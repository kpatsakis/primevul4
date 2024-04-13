static int read_file(const char *path, char *buf, size_t size,
		     struct file_info *d)
{
	size_t linelen = 0, total_len = 0, rv = 0;
	char *line = NULL;
	char *cache = d->buf;
	size_t cache_size = d->buflen;
	FILE *f = fopen(path, "r");
	if (!f)
		return 0;

	while (getline(&line, &linelen, f) != -1) {
		size_t l = snprintf(cache, cache_size, "%s", line);
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

	d->size = total_len;
	if (total_len > size ) total_len = size;

	/* read from off 0 */
	memcpy(buf, d->buf, total_len);
	rv = total_len;
  err:
	fclose(f);
	free(line);
	return rv;
}
