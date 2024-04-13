static int __ap_poll_device(struct ap_device *ap_dev, unsigned long *flags)
{
	if (!ap_dev->unregistered) {
		if (ap_poll_queue(ap_dev, flags))
			ap_dev->unregistered = 1;
		if (ap_dev->reset == AP_RESET_DO)
			ap_reset(ap_dev);
	}
	return 0;
}
