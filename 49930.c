static inline int ap_poll_queue(struct ap_device *ap_dev, unsigned long *flags)
{
	int rc;

	rc = ap_poll_read(ap_dev, flags);
	if (rc)
		return rc;
	return ap_poll_write(ap_dev, flags);
}
