static int hid_start_in(struct hid_device *hid)
{
	unsigned long flags;
	int rc = 0;
	struct usbhid_device *usbhid = hid->driver_data;

	spin_lock_irqsave(&usbhid->lock, flags);
	if (test_bit(HID_IN_POLLING, &usbhid->iofl) &&
	    !test_bit(HID_DISCONNECTED, &usbhid->iofl) &&
	    !test_bit(HID_SUSPENDED, &usbhid->iofl) &&
	    !test_and_set_bit(HID_IN_RUNNING, &usbhid->iofl)) {
		rc = usb_submit_urb(usbhid->urbin, GFP_ATOMIC);
		if (rc != 0) {
			clear_bit(HID_IN_RUNNING, &usbhid->iofl);
			if (rc == -ENOSPC)
				set_bit(HID_NO_BANDWIDTH, &usbhid->iofl);
		} else {
			clear_bit(HID_NO_BANDWIDTH, &usbhid->iofl);
		}
	}
	spin_unlock_irqrestore(&usbhid->lock, flags);
	return rc;
}
