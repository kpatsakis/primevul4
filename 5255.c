int digest_file_by_name(const char *algo, const char *filename,
			unsigned char *hash,
			const unsigned char *sig)
{
	struct digest *d;
	int ret;

	d = digest_alloc(algo);
	if (!d)
		return -EIO;

	ret = digest_file(d, filename, hash, sig);
	digest_free(d);
	return ret;
}