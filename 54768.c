static int is_good_config(struct usbtest_dev *tdev, int len)
{
	struct usb_config_descriptor	*config;

	if (len < sizeof(*config))
		return 0;
	config = (struct usb_config_descriptor *) tdev->buf;

	switch (config->bDescriptorType) {
	case USB_DT_CONFIG:
	case USB_DT_OTHER_SPEED_CONFIG:
		if (config->bLength != 9) {
			ERROR(tdev, "bogus config descriptor length\n");
			return 0;
		}
		/* this bit 'must be 1' but often isn't */
		if (!realworld && !(config->bmAttributes & 0x80)) {
			ERROR(tdev, "high bit of config attributes not set\n");
			return 0;
		}
		if (config->bmAttributes & 0x1f) {	/* reserved == 0 */
			ERROR(tdev, "reserved config bits set\n");
			return 0;
		}
		break;
	default:
		return 0;
	}

	if (le16_to_cpu(config->wTotalLength) == len)	/* read it all */
		return 1;
	if (le16_to_cpu(config->wTotalLength) >= TBUF_SIZE)	/* max partial read */
		return 1;
	ERROR(tdev, "bogus config descriptor read size\n");
	return 0;
}
