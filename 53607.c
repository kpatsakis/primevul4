static void usb_enable_link_state(struct usb_hcd *hcd, struct usb_device *udev,
		enum usb3_link_state state)
{
	int timeout, ret;
	__u8 u1_mel = udev->bos->ss_cap->bU1devExitLat;
	__le16 u2_mel = udev->bos->ss_cap->bU2DevExitLat;

	/* If the device says it doesn't have *any* exit latency to come out of
	 * U1 or U2, it's probably lying.  Assume it doesn't implement that link
	 * state.
	 */
	if ((state == USB3_LPM_U1 && u1_mel == 0) ||
			(state == USB3_LPM_U2 && u2_mel == 0))
		return;

	/*
	 * First, let the device know about the exit latencies
	 * associated with the link state we're about to enable.
	 */
	ret = usb_req_set_sel(udev, state);
	if (ret < 0) {
		dev_warn(&udev->dev, "Set SEL for device-initiated %s failed.\n",
				usb3_lpm_names[state]);
		return;
	}

	/* We allow the host controller to set the U1/U2 timeout internally
	 * first, so that it can change its schedule to account for the
	 * additional latency to send data to a device in a lower power
	 * link state.
	 */
	timeout = hcd->driver->enable_usb3_lpm_timeout(hcd, udev, state);

	/* xHCI host controller doesn't want to enable this LPM state. */
	if (timeout == 0)
		return;

	if (timeout < 0) {
		dev_warn(&udev->dev, "Could not enable %s link state, "
				"xHCI error %i.\n", usb3_lpm_names[state],
				timeout);
		return;
	}

	if (usb_set_lpm_timeout(udev, state, timeout))
		/* If we can't set the parent hub U1/U2 timeout,
		 * device-initiated LPM won't be allowed either, so let the xHCI
		 * host know that this link state won't be enabled.
		 */
		hcd->driver->disable_usb3_lpm_timeout(hcd, udev, state);

	/* Only a configured device will accept the Set Feature U1/U2_ENABLE */
	else if (udev->actconfig)
		usb_set_device_initiated_lpm(udev, state, true);

}
