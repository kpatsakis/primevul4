archive_read_format_7zip_bid(struct archive_read *a, int best_bid)
{
	const char *p;

	/* If someone has already bid more than 32, then avoid
	   trashing the look-ahead buffers with a seek. */
	if (best_bid > 32)
		return (-1);

	if ((p = __archive_read_ahead(a, 6, NULL)) == NULL)
		return (0);

	/* If first six bytes are the 7-Zip signature,
	 * return the bid right now. */
	if (memcmp(p, _7ZIP_SIGNATURE, 6) == 0)
		return (48);

	/*
	 * It may a 7-Zip SFX archive file. If first two bytes are
	 * 'M' and 'Z' available on Windows or first four bytes are
	 * "\x7F\x45LF" available on posix like system, seek the 7-Zip
	 * signature. Although we will perform a seek when reading
	 * a header, what we do not use __archive_read_seek() here is
	 * due to a bidding performance.
	 */
	if ((p[0] == 'M' && p[1] == 'Z') || memcmp(p, "\x7F\x45LF", 4) == 0) {
		ssize_t offset = SFX_MIN_ADDR;
		ssize_t window = 4096;
		ssize_t bytes_avail;
		while (offset + window <= (SFX_MAX_ADDR)) {
			const char *buff = __archive_read_ahead(a,
					offset + window, &bytes_avail);
			if (buff == NULL) {
				/* Remaining bytes are less than window. */
				window >>= 1;
				if (window < 0x40)
					return (0);
				continue;
			}
			p = buff + offset;
			while (p + 32 < buff + bytes_avail) {
				int step = check_7zip_header_in_sfx(p);
				if (step == 0)
					return (48);
				p += step;
			}
			offset = p - buff;
		}
	}
	return (0);
}
