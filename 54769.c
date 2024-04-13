static int is_good_ext(struct usbtest_dev *tdev, u8 *buf)
{
	struct usb_ext_cap_descriptor *ext;
	u32 attr;

	ext = (struct usb_ext_cap_descriptor *) buf;

	if (ext->bLength != USB_DT_USB_EXT_CAP_SIZE) {
		ERROR(tdev, "bogus usb 2.0 extension descriptor length\n");
		return 0;
	}

	attr = le32_to_cpu(ext->bmAttributes);
	/* bits[1:15] is used and others are reserved */
	if (attr & ~0xfffe) {	/* reserved == 0 */
		ERROR(tdev, "reserved bits set\n");
		return 0;
	}

	return 1;
}
