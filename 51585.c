archive_read_format_iso9660_bid(struct archive_read *a, int best_bid)
{
	struct iso9660 *iso9660;
	ssize_t bytes_read;
	const unsigned char *p;
	int seenTerminator;

	/* If there's already a better bid than we can ever
	   make, don't bother testing. */
	if (best_bid > 48)
		return (-1);

	iso9660 = (struct iso9660 *)(a->format->data);

	/*
	 * Skip the first 32k (reserved area) and get the first
	 * 8 sectors of the volume descriptor table.  Of course,
	 * if the I/O layer gives us more, we'll take it.
	 */
#define RESERVED_AREA	(SYSTEM_AREA_BLOCK * LOGICAL_BLOCK_SIZE)
	p = __archive_read_ahead(a,
	    RESERVED_AREA + 8 * LOGICAL_BLOCK_SIZE,
	    &bytes_read);
	if (p == NULL)
	    return (-1);

	/* Skip the reserved area. */
	bytes_read -= RESERVED_AREA;
	p += RESERVED_AREA;

	/* Check each volume descriptor. */
	seenTerminator = 0;
	for (; bytes_read > LOGICAL_BLOCK_SIZE;
	    bytes_read -= LOGICAL_BLOCK_SIZE, p += LOGICAL_BLOCK_SIZE) {
		/* Do not handle undefined Volume Descriptor Type. */
		if (p[0] >= 4 && p[0] <= 254)
			return (0);
		/* Standard Identifier must be "CD001" */
		if (memcmp(p + 1, "CD001", 5) != 0)
			return (0);
		if (isPVD(iso9660, p))
			continue;
		if (!iso9660->joliet.location) {
			if (isJolietSVD(iso9660, p))
				continue;
		}
		if (isBootRecord(iso9660, p))
			continue;
		if (isEVD(iso9660, p))
			continue;
		if (isSVD(iso9660, p))
			continue;
		if (isVolumePartition(iso9660, p))
			continue;
		if (isVDSetTerminator(iso9660, p)) {
			seenTerminator = 1;
			break;
		}
		return (0);
	}
	/*
	 * ISO 9660 format must have Primary Volume Descriptor and
	 * Volume Descriptor Set Terminator.
	 */
	if (seenTerminator && iso9660->primary.location > 16)
		return (48);

	/* We didn't find a valid PVD; return a bid of zero. */
	return (0);
}
