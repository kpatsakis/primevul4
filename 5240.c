int digest_file_window(struct digest *d, const char *filename,
		       unsigned char *hash,
		       const unsigned char *sig,
		       loff_t start, loff_t size)
{
	int fd, ret;

	ret = digest_init(d);
	if (ret)
		return ret;

	fd = open(filename, O_RDONLY);
	if (fd < 0) {
		perror(filename);
		return -errno;
	}

	ret = digest_update_from_fd(d, fd, start, size);
	if (ret)
		goto out;

	if (sig)
		ret = digest_verify(d, sig);
	else
		ret = digest_final(d, hash);
out:
	close(fd);

	return ret;
}