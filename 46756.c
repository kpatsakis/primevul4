sync_max_store(struct mddev *mddev, const char *buf, size_t len)
{
	unsigned int max;
	int rv;

	if (strncmp(buf, "system", 6)==0) {
		max = 0;
	} else {
		rv = kstrtouint(buf, 10, &max);
		if (rv < 0)
			return rv;
		if (max == 0)
			return -EINVAL;
	}
	mddev->sync_speed_max = max;
	return len;
}
