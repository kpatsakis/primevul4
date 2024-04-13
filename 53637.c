static int usb_set_lpm_timeout(struct usb_device *udev,
		enum usb3_link_state state, int timeout)
{
	int ret;
	int feature;

	switch (state) {
	case USB3_LPM_U1:
		feature = USB_PORT_FEAT_U1_TIMEOUT;
		break;
	case USB3_LPM_U2:
		feature = USB_PORT_FEAT_U2_TIMEOUT;
		break;
	default:
		dev_warn(&udev->dev, "%s: Can't set timeout for non-U1 or U2 state.\n",
				__func__);
		return -EINVAL;
	}

	if (state == USB3_LPM_U1 && timeout > USB3_LPM_U1_MAX_TIMEOUT &&
			timeout != USB3_LPM_DEVICE_INITIATED) {
		dev_warn(&udev->dev, "Failed to set %s timeout to 0x%x, "
				"which is a reserved value.\n",
				usb3_lpm_names[state], timeout);
		return -EINVAL;
	}

	ret = set_port_feature(udev->parent,
			USB_PORT_LPM_TIMEOUT(timeout) | udev->portnum,
			feature);
	if (ret < 0) {
		dev_warn(&udev->dev, "Failed to set %s timeout to 0x%x,"
				"error code %i\n", usb3_lpm_names[state],
				timeout, ret);
		return -EBUSY;
	}
	if (state == USB3_LPM_U1)
		udev->u1_params.timeout = timeout;
	else
		udev->u2_params.timeout = timeout;
	return 0;
}
