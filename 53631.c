static int usb_set_device_initiated_lpm(struct usb_device *udev,
		enum usb3_link_state state, bool enable)
{
	int ret;
	int feature;

	switch (state) {
	case USB3_LPM_U1:
		feature = USB_DEVICE_U1_ENABLE;
		break;
	case USB3_LPM_U2:
		feature = USB_DEVICE_U2_ENABLE;
		break;
	default:
		dev_warn(&udev->dev, "%s: Can't %s non-U1 or U2 state.\n",
				__func__, enable ? "enable" : "disable");
		return -EINVAL;
	}

	if (udev->state != USB_STATE_CONFIGURED) {
		dev_dbg(&udev->dev, "%s: Can't %s %s state "
				"for unconfigured device.\n",
				__func__, enable ? "enable" : "disable",
				usb3_lpm_names[state]);
		return 0;
	}

	if (enable) {
		/*
		 * Now send the control transfer to enable device-initiated LPM
		 * for either U1 or U2.
		 */
		ret = usb_control_msg(udev, usb_sndctrlpipe(udev, 0),
				USB_REQ_SET_FEATURE,
				USB_RECIP_DEVICE,
				feature,
				0, NULL, 0,
				USB_CTRL_SET_TIMEOUT);
	} else {
		ret = usb_control_msg(udev, usb_sndctrlpipe(udev, 0),
				USB_REQ_CLEAR_FEATURE,
				USB_RECIP_DEVICE,
				feature,
				0, NULL, 0,
				USB_CTRL_SET_TIMEOUT);
	}
	if (ret < 0) {
		dev_warn(&udev->dev, "%s of device-initiated %s failed.\n",
				enable ? "Enable" : "Disable",
				usb3_lpm_names[state]);
		return -EBUSY;
	}
	return 0;
}
