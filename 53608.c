void usb_enable_lpm(struct usb_device *udev)
{
	struct usb_hcd *hcd;

	if (!udev || !udev->parent ||
			udev->speed != USB_SPEED_SUPER ||
			!udev->lpm_capable ||
			udev->state < USB_STATE_DEFAULT)
		return;

	udev->lpm_disable_count--;
	hcd = bus_to_hcd(udev->bus);
	/* Double check that we can both enable and disable LPM.
	 * Device must be configured to accept set feature U1/U2 timeout.
	 */
	if (!hcd || !hcd->driver->enable_usb3_lpm_timeout ||
			!hcd->driver->disable_usb3_lpm_timeout)
		return;

	if (udev->lpm_disable_count > 0)
		return;

	usb_enable_link_state(hcd, udev, USB3_LPM_U1);
	usb_enable_link_state(hcd, udev, USB3_LPM_U2);

	udev->usb3_lpm_enabled = 1;
}
