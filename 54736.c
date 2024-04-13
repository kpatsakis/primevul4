static int hid_submit_ctrl(struct hid_device *hid)
{
	struct hid_report *report;
	unsigned char dir;
	char *raw_report;
	int len, r;
	struct usbhid_device *usbhid = hid->driver_data;

	report = usbhid->ctrl[usbhid->ctrltail].report;
	raw_report = usbhid->ctrl[usbhid->ctrltail].raw_report;
	dir = usbhid->ctrl[usbhid->ctrltail].dir;

	len = ((report->size - 1) >> 3) + 1 + (report->id > 0);
	if (dir == USB_DIR_OUT) {
		usbhid->urbctrl->pipe = usb_sndctrlpipe(hid_to_usb_dev(hid), 0);
		usbhid->urbctrl->transfer_buffer_length = len;
		if (raw_report) {
			memcpy(usbhid->ctrlbuf, raw_report, len);
			kfree(raw_report);
			usbhid->ctrl[usbhid->ctrltail].raw_report = NULL;
		}
	} else {
		int maxpacket, padlen;

		usbhid->urbctrl->pipe = usb_rcvctrlpipe(hid_to_usb_dev(hid), 0);
		maxpacket = usb_maxpacket(hid_to_usb_dev(hid),
					  usbhid->urbctrl->pipe, 0);
		if (maxpacket > 0) {
			padlen = DIV_ROUND_UP(len, maxpacket);
			padlen *= maxpacket;
			if (padlen > usbhid->bufsize)
				padlen = usbhid->bufsize;
		} else
			padlen = 0;
		usbhid->urbctrl->transfer_buffer_length = padlen;
	}
	usbhid->urbctrl->dev = hid_to_usb_dev(hid);

	usbhid->cr->bRequestType = USB_TYPE_CLASS | USB_RECIP_INTERFACE | dir;
	usbhid->cr->bRequest = (dir == USB_DIR_OUT) ? HID_REQ_SET_REPORT :
						      HID_REQ_GET_REPORT;
	usbhid->cr->wValue = cpu_to_le16(((report->type + 1) << 8) |
					 report->id);
	usbhid->cr->wIndex = cpu_to_le16(usbhid->ifnum);
	usbhid->cr->wLength = cpu_to_le16(len);

	dbg_hid("submitting ctrl urb: %s wValue=0x%04x wIndex=0x%04x wLength=%u\n",
		usbhid->cr->bRequest == HID_REQ_SET_REPORT ? "Set_Report" :
							     "Get_Report",
		usbhid->cr->wValue, usbhid->cr->wIndex, usbhid->cr->wLength);

	r = usb_submit_urb(usbhid->urbctrl, GFP_ATOMIC);
	if (r < 0) {
		hid_err(hid, "usb_submit_urb(ctrl) failed: %d\n", r);
		return r;
	}
	usbhid->last_ctrl = jiffies;
	return 0;
}
