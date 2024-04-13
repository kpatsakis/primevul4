static int is_good_ss_cap(struct usbtest_dev *tdev, u8 *buf)
{
	struct usb_ss_cap_descriptor *ss;

	ss = (struct usb_ss_cap_descriptor *) buf;

	if (ss->bLength != USB_DT_USB_SS_CAP_SIZE) {
		ERROR(tdev, "bogus superspeed device capability descriptor length\n");
		return 0;
	}

	/*
	 * only bit[1] of bmAttributes is used for LTM and others are
	 * reserved
	 */
	if (ss->bmAttributes & ~0x02) {	/* reserved == 0 */
		ERROR(tdev, "reserved bits set in bmAttributes\n");
		return 0;
	}

	/* bits[0:3] of wSpeedSupported is used and others are reserved */
	if (le16_to_cpu(ss->wSpeedSupported) & ~0x0f) {	/* reserved == 0 */
		ERROR(tdev, "reserved bits set in wSpeedSupported\n");
		return 0;
	}

	return 1;
}
