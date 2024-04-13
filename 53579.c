static int hub_set_address(struct usb_device *udev, int devnum)
{
	int retval;
	struct usb_hcd *hcd = bus_to_hcd(udev->bus);

	/*
	 * The host controller will choose the device address,
	 * instead of the core having chosen it earlier
	 */
	if (!hcd->driver->address_device && devnum <= 1)
		return -EINVAL;
	if (udev->state == USB_STATE_ADDRESS)
		return 0;
	if (udev->state != USB_STATE_DEFAULT)
		return -EINVAL;
	if (hcd->driver->address_device)
		retval = hcd->driver->address_device(hcd, udev);
	else
		retval = usb_control_msg(udev, usb_sndaddr0pipe(),
				USB_REQ_SET_ADDRESS, 0, devnum, 0,
				NULL, 0, USB_CTRL_SET_TIMEOUT);
	if (retval == 0) {
		update_devnum(udev, devnum);
		/* Device now using proper address. */
		usb_set_device_state(udev, USB_STATE_ADDRESS);
		usb_ep0_reinit(udev);
	}
	return retval;
}
