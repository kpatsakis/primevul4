static ssize_t ap_poll_thread_store(struct bus_type *bus,
				    const char *buf, size_t count)
{
	int flag, rc;

	if (sscanf(buf, "%d\n", &flag) != 1)
		return -EINVAL;
	if (flag) {
		rc = ap_poll_thread_start();
		if (rc)
			return rc;
	}
	else
		ap_poll_thread_stop();
	return count;
}
