static bool use_new_scheme(struct usb_device *udev, int retry)
{
	if (udev->speed == USB_SPEED_SUPER)
		return false;

	return USE_NEW_SCHEME(retry);
}
