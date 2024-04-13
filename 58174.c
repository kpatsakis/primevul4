static int dvb_usbv2_download_firmware(struct dvb_usb_device *d,
		const char *name)
{
	int ret;
	const struct firmware *fw;
	dev_dbg(&d->udev->dev, "%s:\n", __func__);

	if (!d->props->download_firmware) {
		ret = -EINVAL;
		goto err;
	}

	ret = request_firmware(&fw, name, &d->udev->dev);
	if (ret < 0) {
		dev_err(&d->udev->dev,
				"%s: Did not find the firmware file '%s'. Please see linux/Documentation/dvb/ for more details on firmware-problems. Status %d\n",
				KBUILD_MODNAME, name, ret);
		goto err;
	}

	dev_info(&d->udev->dev, "%s: downloading firmware from file '%s'\n",
			KBUILD_MODNAME, name);

	ret = d->props->download_firmware(d, fw);
	release_firmware(fw);
	if (ret < 0)
		goto err;

	return ret;
err:
	dev_dbg(&d->udev->dev, "%s: failed=%d\n", __func__, ret);
	return ret;
}
