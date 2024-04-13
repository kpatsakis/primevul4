read_Digests(struct archive_read *a, struct _7z_digests *d, size_t num)
{
	const unsigned char *p;
	unsigned i;

	if (num == 0)
		return (-1);
	memset(d, 0, sizeof(*d));

	d->defineds = malloc(num);
	if (d->defineds == NULL)
		return (-1);
	/*
	 * Read Bools.
	 */
	if ((p = header_bytes(a, 1)) == NULL)
		return (-1);
	if (*p == 0) {
		if (read_Bools(a, d->defineds, num) < 0)
			return (-1);
	} else
		/* All are defined */
		memset(d->defineds, 1, num);

	d->digests = calloc(num, sizeof(*d->digests));
	if (d->digests == NULL)
		return (-1);
	for (i = 0; i < num; i++) {
		if (d->defineds[i]) {
			if ((p = header_bytes(a, 4)) == NULL)
				return (-1);
			d->digests[i] = archive_le32dec(p);
		}
	}

	return (0);
}
