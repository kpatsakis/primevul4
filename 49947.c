static void ap_request_timeout(unsigned long data)
{
	struct ap_device *ap_dev = (struct ap_device *) data;

	if (ap_dev->reset == AP_RESET_ARMED) {
		ap_dev->reset = AP_RESET_DO;

		if (ap_using_interrupts())
			tasklet_schedule(&ap_tasklet);
	}
}
