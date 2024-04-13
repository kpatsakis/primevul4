int lz4_decompress(const unsigned char *src, size_t *src_len,
		unsigned char *dest, size_t actual_dest_len)
{
	int ret = -1;
	int input_len = 0;

	input_len = lz4_uncompress(src, dest, actual_dest_len);
	if (input_len < 0)
		goto exit_0;
	*src_len = input_len;

	return 0;
exit_0:
	return ret;
}
