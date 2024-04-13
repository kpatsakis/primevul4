max_corrected_read_errors_store(struct mddev *mddev, const char *buf, size_t len)
{
	unsigned int n;
	int rv;

	rv = kstrtouint(buf, 10, &n);
	if (rv < 0)
		return rv;
	atomic_set(&mddev->max_corr_read_errors, n);
	return len;
}
