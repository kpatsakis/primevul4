static int verify_not_halted(struct usbtest_dev *tdev, int ep, struct urb *urb)
{
	int	retval;
	u16	status;

	/* shouldn't look or act halted */
	retval = usb_get_status(urb->dev, USB_RECIP_ENDPOINT, ep, &status);
	if (retval < 0) {
		ERROR(tdev, "ep %02x couldn't get no-halt status, %d\n",
				ep, retval);
		return retval;
	}
	if (status != 0) {
		ERROR(tdev, "ep %02x bogus status: %04x != 0\n", ep, status);
		return -EINVAL;
	}
	retval = simple_io(tdev, urb, 1, 0, 0, __func__);
	if (retval != 0)
		return -EINVAL;
	return 0;
}
