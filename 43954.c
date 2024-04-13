static int whiteheat_firmware_download(struct usb_serial *serial,
					const struct usb_device_id *id)
{
	int response;

	response = ezusb_fx1_ihex_firmware_download(serial->dev, "whiteheat_loader.fw");
	if (response >= 0) {
		response = ezusb_fx1_ihex_firmware_download(serial->dev, "whiteheat.fw");
		if (response >= 0)
			return 0;
	}
	return -ENOENT;
}
