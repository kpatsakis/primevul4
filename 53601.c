static int usb_disable_link_state(struct usb_hcd *hcd, struct usb_device *udev,
		enum usb3_link_state state)
{
	switch (state) {
	case USB3_LPM_U1:
	case USB3_LPM_U2:
		break;
	default:
		dev_warn(&udev->dev, "%s: Can't disable non-U1 or U2 state.\n",
				__func__);
		return -EINVAL;
	}

	if (usb_set_lpm_timeout(udev, state, 0))
		return -EBUSY;

	usb_set_device_initiated_lpm(udev, state, false);

	if (hcd->driver->disable_usb3_lpm_timeout(hcd, udev, state))
		dev_warn(&udev->dev, "Could not disable xHCI %s timeout, "
				"bus schedule bandwidth may be impacted.\n",
				usb3_lpm_names[state]);
	return 0;
}
