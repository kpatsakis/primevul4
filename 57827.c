static int download_fw(struct edgeport_serial *serial)
{
	struct device *dev = &serial->serial->interface->dev;
	int status = 0;
	struct usb_interface_descriptor *interface;
	const struct firmware *fw;
	const char *fw_name = "edgeport/down3.bin";
	struct edgeport_fw_hdr *fw_hdr;

	status = request_firmware(&fw, fw_name, dev);
	if (status) {
		dev_err(dev, "Failed to load image \"%s\" err %d\n",
				fw_name, status);
		return status;
	}

	if (check_fw_sanity(serial, fw)) {
		status = -EINVAL;
		goto out;
	}

	fw_hdr = (struct edgeport_fw_hdr *)fw->data;

	/* If on-board version is newer, "fw_version" will be updated later. */
	serial->fw_version = (fw_hdr->major_version << 8) +
			fw_hdr->minor_version;

	/*
	 * This routine is entered by both the BOOT mode and the Download mode
	 * We can determine which code is running by the reading the config
	 * descriptor and if we have only one bulk pipe it is in boot mode
	 */
	serial->product_info.hardware_type = HARDWARE_TYPE_TIUMP;

	/* Default to type 2 i2c */
	serial->TI_I2C_Type = DTK_ADDR_SPACE_I2C_TYPE_II;

	status = choose_config(serial->serial->dev);
	if (status)
		goto out;

	interface = &serial->serial->interface->cur_altsetting->desc;
	if (!interface) {
		dev_err(dev, "%s - no interface set, error!\n", __func__);
		status = -ENODEV;
		goto out;
	}

	/*
	 * Setup initial mode -- the default mode 0 is TI_MODE_CONFIGURING
	 * if we have more than one endpoint we are definitely in download
	 * mode
	 */
	if (interface->bNumEndpoints > 1) {
		serial->product_info.TiMode = TI_MODE_DOWNLOAD;
		status = do_download_mode(serial, fw);
	} else {
		/* Otherwise we will remain in configuring mode */
		serial->product_info.TiMode = TI_MODE_CONFIGURING;
		status = do_boot_mode(serial, fw);
	}

out:
	release_firmware(fw);
	return status;
}
