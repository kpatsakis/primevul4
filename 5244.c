int digest_file(struct digest *d, const char *filename,
		unsigned char *hash,
		const unsigned char *sig)
{
	struct stat st;

	if (stat(filename, &st))
		return -errno;

	return digest_file_window(d, filename, hash, sig, 0, st.st_size);
}