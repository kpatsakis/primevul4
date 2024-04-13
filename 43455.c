int lz4_decompress_unknownoutputsize(const unsigned char *src, size_t src_len,
		unsigned char *dest, size_t *dest_len)
{
	int ret = -1;
	int out_len = 0;

	out_len = lz4_uncompress_unknownoutputsize(src, dest, src_len,
					*dest_len);
	if (out_len < 0)
		goto exit_0;
	*dest_len = out_len;

	return 0;
exit_0:
	return ret;
}
