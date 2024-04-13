sync_min_store(struct mddev *mddev, const char *buf, size_t len)
{
	unsigned int min;
	int rv;

	if (strncmp(buf, "system", 6)==0) {
		min = 0;
	} else {
		rv = kstrtouint(buf, 10, &min);
		if (rv < 0)
			return rv;
		if (min == 0)
			return -EINVAL;
	}
	mddev->sync_speed_min = min;
	return len;
}
