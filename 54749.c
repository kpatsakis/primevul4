static int usbhid_restart_out_queue(struct usbhid_device *usbhid)
{
	struct hid_device *hid = usb_get_intfdata(usbhid->intf);
	int kicked;
	int r;

	if (!hid || test_bit(HID_RESET_PENDING, &usbhid->iofl) ||
			test_bit(HID_SUSPENDED, &usbhid->iofl))
		return 0;

	if ((kicked = (usbhid->outhead != usbhid->outtail))) {
		hid_dbg(hid, "Kicking head %d tail %d", usbhid->outhead, usbhid->outtail);

		/* Try to wake up from autosuspend... */
		r = usb_autopm_get_interface_async(usbhid->intf);
		if (r < 0)
			return r;

		/*
		 * If still suspended, don't submit.  Submission will
		 * occur if/when resume drains the queue.
		 */
		if (test_bit(HID_SUSPENDED, &usbhid->iofl)) {
			usb_autopm_put_interface_no_suspend(usbhid->intf);
			return r;
		}

		/* Asynchronously flush queue. */
		set_bit(HID_OUT_RUNNING, &usbhid->iofl);
		if (hid_submit_out(hid)) {
			clear_bit(HID_OUT_RUNNING, &usbhid->iofl);
			usb_autopm_put_interface_async(usbhid->intf);
		}
		wake_up(&usbhid->wait);
	}
	return kicked;
}
