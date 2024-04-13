static int cp2112_get_usb_config(struct hid_device *hdev,
				 struct cp2112_usb_config_report *cfg)
{
	int ret;

	ret = cp2112_hid_get(hdev, CP2112_USB_CONFIG, (u8 *)cfg, sizeof(*cfg),
			     HID_FEATURE_REPORT);
	if (ret != sizeof(*cfg)) {
		hid_err(hdev, "error reading usb config: %d\n", ret);
		if (ret < 0)
			return ret;
		return -EIO;
	}

	return 0;
}
