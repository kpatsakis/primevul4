rd_contents_init(struct archive_read *a, enum enctype encoding,
    int a_sum_alg, int e_sum_alg)
{
	int r;

	/* Init decompress library. */
	if ((r = decompression_init(a, encoding)) != ARCHIVE_OK)
		return (r);
	/* Init checksum library. */
	checksum_init(a, a_sum_alg, e_sum_alg);
	return (ARCHIVE_OK);
}
