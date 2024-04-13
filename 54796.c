static int verify_halted(struct usbtest_dev *tdev, int ep, struct urb *urb)
{
	int	retval;
	u16	status;

	/* should look and act halted */
	retval = usb_get_status(urb->dev, USB_RECIP_ENDPOINT, ep, &status);
	if (retval < 0) {
		ERROR(tdev, "ep %02x couldn't get halt status, %d\n",
				ep, retval);
		return retval;
	}
	if (status != 1) {
		ERROR(tdev, "ep %02x bogus status: %04x != 1\n", ep, status);
		return -EINVAL;
	}
	retval = simple_io(tdev, urb, 1, 0, -EPIPE, __func__);
	if (retval != -EPIPE)
		return -EINVAL;
	retval = simple_io(tdev, urb, 1, 0, -EPIPE, "verify_still_halted");
	if (retval != -EPIPE)
		return -EINVAL;
	return 0;
}
