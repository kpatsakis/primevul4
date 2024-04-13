static int is_good_con_id(struct usbtest_dev *tdev, u8 *buf)
{
	struct usb_ss_container_id_descriptor *con_id;

	con_id = (struct usb_ss_container_id_descriptor *) buf;

	if (con_id->bLength != USB_DT_USB_SS_CONTN_ID_SIZE) {
		ERROR(tdev, "bogus container id descriptor length\n");
		return 0;
	}

	if (con_id->bReserved) {	/* reserved == 0 */
		ERROR(tdev, "reserved bits set\n");
		return 0;
	}

	return 1;
}
