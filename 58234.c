int dvb_usb_download_firmware(struct usb_device *udev, struct dvb_usb_device_properties *props)
{
	int ret;
	const struct firmware *fw = NULL;

	if ((ret = request_firmware(&fw, props->firmware, &udev->dev)) != 0) {
		err("did not find the firmware file. (%s) Please see linux/Documentation/dvb/ for more details on firmware-problems. (%d)",
			props->firmware,ret);
		return ret;
	}

	info("downloading firmware from file '%s'",props->firmware);

	switch (props->usb_ctrl) {
		case CYPRESS_AN2135:
		case CYPRESS_AN2235:
		case CYPRESS_FX2:
			ret = usb_cypress_load_firmware(udev, fw, props->usb_ctrl);
			break;
		case DEVICE_SPECIFIC:
			if (props->download_firmware)
				ret = props->download_firmware(udev,fw);
			else {
				err("BUG: driver didn't specified a download_firmware-callback, although it claims to have a DEVICE_SPECIFIC one.");
				ret = -EINVAL;
			}
			break;
		default:
			ret = -EINVAL;
			break;
	}

	release_firmware(fw);
	return ret;
}
