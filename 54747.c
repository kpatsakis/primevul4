static void usbhid_request(struct hid_device *hid, struct hid_report *rep, int reqtype)
{
	switch (reqtype) {
	case HID_REQ_GET_REPORT:
		usbhid_submit_report(hid, rep, USB_DIR_IN);
		break;
	case HID_REQ_SET_REPORT:
		usbhid_submit_report(hid, rep, USB_DIR_OUT);
		break;
	}
}
