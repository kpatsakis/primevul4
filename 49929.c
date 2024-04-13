static void ap_poll_all(unsigned long dummy)
{
	unsigned long flags;
	struct ap_device *ap_dev;

	/* Reset the indicator if interrupts are used. Thus new interrupts can
	 * be received. Doing it in the beginning of the tasklet is therefor
	 * important that no requests on any AP get lost.
	 */
	if (ap_using_interrupts())
		xchg(ap_airq.lsi_ptr, 0);
	do {
		flags = 0;
		spin_lock(&ap_device_list_lock);
		list_for_each_entry(ap_dev, &ap_device_list, list) {
			spin_lock(&ap_dev->lock);
			__ap_poll_device(ap_dev, &flags);
			spin_unlock(&ap_dev->lock);
		}
		spin_unlock(&ap_device_list_lock);
	} while (flags & 1);
	if (flags & 2)
		ap_schedule_poll_timer();
}
