skip_sfx(struct archive_read *a, ssize_t bytes_avail)
{
	const void *h;
	const char *p, *q;
	size_t skip, offset;
	ssize_t bytes, window;

	/*
	 * If bytes_avail > SFX_MIN_ADDR we do not have to call
	 * __archive_read_seek() at this time since we have
	 * alredy had enough data.
	 */
	if (bytes_avail > SFX_MIN_ADDR)
		__archive_read_consume(a, SFX_MIN_ADDR);
	else if (__archive_read_seek(a, SFX_MIN_ADDR, SEEK_SET) < 0)
		return (ARCHIVE_FATAL);

	offset = 0;
	window = 1;
	while (offset + window <= SFX_MAX_ADDR - SFX_MIN_ADDR) {
		h = __archive_read_ahead(a, window, &bytes);
		if (h == NULL) {
			/* Remaining bytes are less than window. */
			window >>= 1;
			if (window < 0x40)
				goto fatal;
			continue;
		}
		if (bytes < 6) {
			/* This case might happen when window == 1. */
			window = 4096;
			continue;
		}
		p = (const char *)h;
		q = p + bytes;

		/*
		 * Scan ahead until we find something that looks
		 * like the 7-Zip header.
		 */
		while (p + 32 < q) {
			int step = check_7zip_header_in_sfx(p);
			if (step == 0) {
				struct _7zip *zip =
				    (struct _7zip *)a->format->data;
				skip = p - (const char *)h;
				__archive_read_consume(a, skip);
				zip->seek_base = SFX_MIN_ADDR + offset + skip;
				return (ARCHIVE_OK);
			}
			p += step;
		}
		skip = p - (const char *)h;
		__archive_read_consume(a, skip);
		offset += skip;
		if (window == 1)
			window = 4096;
	}
fatal:
	archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
	    "Couldn't find out 7-Zip header");
	return (ARCHIVE_FATAL);
}
