static void ap_increase_queue_count(struct ap_device *ap_dev)
{
	int timeout = ap_dev->drv->request_timeout;

	ap_dev->queue_count++;
	if (ap_dev->queue_count == 1) {
		mod_timer(&ap_dev->timeout, jiffies + timeout);
		ap_dev->reset = AP_RESET_ARMED;
	}
}
