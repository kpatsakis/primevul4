static size_t ecryptfs_max_decoded_size(size_t encoded_size)
{
	/* Not exact; conservatively long. Every block of 4
	 * encoded characters decodes into a block of 3
	 * decoded characters. This segment of code provides
	 * the caller with the maximum amount of allocated
	 * space that @dst will need to point to in a
	 * subsequent call. */
	return ((encoded_size + 1) * 3) / 4;
}
