xar_bid(struct archive_read *a, int best_bid)
{
	const unsigned char *b;
	int bid;

	(void)best_bid; /* UNUSED */

	b = __archive_read_ahead(a, HEADER_SIZE, NULL);
	if (b == NULL)
		return (-1);

	bid = 0;
	/*
	 * Verify magic code
	 */
	if (archive_be32dec(b) != HEADER_MAGIC)
		return (0);
	bid += 32;
	/*
	 * Verify header size
	 */
	if (archive_be16dec(b+4) != HEADER_SIZE)
		return (0);
	bid += 16;
	/*
	 * Verify header version
	 */
	if (archive_be16dec(b+6) != HEADER_VERSION)
		return (0);
	bid += 16;
	/*
	 * Verify type of checksum
	 */
	switch (archive_be32dec(b+24)) {
	case CKSUM_NONE:
	case CKSUM_SHA1:
	case CKSUM_MD5:
		bid += 32;
		break;
	default:
		return (0);
	}

	return (bid);
}
