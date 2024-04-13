static int hid_submit_out(struct hid_device *hid)
{
	struct hid_report *report;
	char *raw_report;
	struct usbhid_device *usbhid = hid->driver_data;
	int r;

	report = usbhid->out[usbhid->outtail].report;
	raw_report = usbhid->out[usbhid->outtail].raw_report;

	usbhid->urbout->transfer_buffer_length = hid_report_len(report);
	usbhid->urbout->dev = hid_to_usb_dev(hid);
	if (raw_report) {
		memcpy(usbhid->outbuf, raw_report,
				usbhid->urbout->transfer_buffer_length);
		kfree(raw_report);
		usbhid->out[usbhid->outtail].raw_report = NULL;
	}

	dbg_hid("submitting out urb\n");

	r = usb_submit_urb(usbhid->urbout, GFP_ATOMIC);
	if (r < 0) {
		hid_err(hid, "usb_submit_urb(out) failed: %d\n", r);
		return r;
	}
	usbhid->last_out = jiffies;
	return 0;
}
