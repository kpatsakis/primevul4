int usb_disable_lpm(struct usb_device *udev)
{
	struct usb_hcd *hcd;

	if (!udev || !udev->parent ||
			udev->speed != USB_SPEED_SUPER ||
			!udev->lpm_capable ||
			udev->state < USB_STATE_DEFAULT)
		return 0;

	hcd = bus_to_hcd(udev->bus);
	if (!hcd || !hcd->driver->disable_usb3_lpm_timeout)
		return 0;

	udev->lpm_disable_count++;
	if ((udev->u1_params.timeout == 0 && udev->u2_params.timeout == 0))
		return 0;

	/* If LPM is enabled, attempt to disable it. */
	if (usb_disable_link_state(hcd, udev, USB3_LPM_U1))
		goto enable_lpm;
	if (usb_disable_link_state(hcd, udev, USB3_LPM_U2))
		goto enable_lpm;

	udev->usb3_lpm_enabled = 0;

	return 0;

enable_lpm:
	usb_enable_lpm(udev);
	return -EBUSY;
}
