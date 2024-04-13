static int usbhid_output_report(struct hid_device *hid, __u8 *buf, size_t count)
{
	struct usbhid_device *usbhid = hid->driver_data;
	struct usb_device *dev = hid_to_usb_dev(hid);
	int actual_length, skipped_report_id = 0, ret;

	if (!usbhid->urbout)
		return -ENOSYS;

	if (buf[0] == 0x0) {
		/* Don't send the Report ID */
		buf++;
		count--;
		skipped_report_id = 1;
	}

	ret = usb_interrupt_msg(dev, usbhid->urbout->pipe,
				buf, count, &actual_length,
				USB_CTRL_SET_TIMEOUT);
	/* return the number of bytes transferred */
	if (ret == 0) {
		ret = actual_length;
		/* count also the report id */
		if (skipped_report_id)
			ret++;
	}

	return ret;
}
