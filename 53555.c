static int hub_enable_device(struct usb_device *udev)
{
	struct usb_hcd *hcd = bus_to_hcd(udev->bus);

	if (!hcd->driver->enable_device)
		return 0;
	if (udev->state == USB_STATE_ADDRESS)
		return 0;
	if (udev->state != USB_STATE_DEFAULT)
		return -EINVAL;

	return hcd->driver->enable_device(hcd, udev);
}
