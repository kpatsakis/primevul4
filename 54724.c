static void hid_io_error(struct hid_device *hid)
{
	unsigned long flags;
	struct usbhid_device *usbhid = hid->driver_data;

	spin_lock_irqsave(&usbhid->lock, flags);

	/* Stop when disconnected */
	if (test_bit(HID_DISCONNECTED, &usbhid->iofl))
		goto done;

	/* If it has been a while since the last error, we'll assume
	 * this a brand new error and reset the retry timeout. */
	if (time_after(jiffies, usbhid->stop_retry + HZ/2))
		usbhid->retry_delay = 0;

	/* When an error occurs, retry at increasing intervals */
	if (usbhid->retry_delay == 0) {
		usbhid->retry_delay = 13;	/* Then 26, 52, 104, 104, ... */
		usbhid->stop_retry = jiffies + msecs_to_jiffies(1000);
	} else if (usbhid->retry_delay < 100)
		usbhid->retry_delay *= 2;

	if (time_after(jiffies, usbhid->stop_retry)) {

		/* Retries failed, so do a port reset unless we lack bandwidth*/
		if (!test_bit(HID_NO_BANDWIDTH, &usbhid->iofl)
		     && !test_and_set_bit(HID_RESET_PENDING, &usbhid->iofl)) {

			schedule_work(&usbhid->reset_work);
			goto done;
		}
	}

	mod_timer(&usbhid->io_retry,
			jiffies + msecs_to_jiffies(usbhid->retry_delay));
done:
	spin_unlock_irqrestore(&usbhid->lock, flags);
}
