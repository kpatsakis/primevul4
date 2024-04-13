void usb_enable_ltm(struct usb_device *udev)
{
	struct usb_hcd *hcd = bus_to_hcd(udev->bus);

	/* Check if the roothub and device supports LTM. */
	if (!usb_device_supports_ltm(hcd->self.root_hub) ||
			!usb_device_supports_ltm(udev))
		return;

	/* Set Feature LTM Enable can only be sent if the device is
	 * configured.
	 */
	if (!udev->actconfig)
		return;

	usb_control_msg(udev, usb_sndctrlpipe(udev, 0),
			USB_REQ_SET_FEATURE, USB_RECIP_DEVICE,
			USB_DEVICE_LTM_ENABLE, 0, NULL, 0,
			USB_CTRL_SET_TIMEOUT);
}
