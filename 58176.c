static int dvb_usbv2_init(struct dvb_usb_device *d)
{
	int ret;
	dev_dbg(&d->udev->dev, "%s:\n", __func__);

	dvb_usbv2_device_power_ctrl(d, 1);

	if (d->props->read_config) {
		ret = d->props->read_config(d);
		if (ret < 0)
			goto err;
	}

	ret = dvb_usbv2_i2c_init(d);
	if (ret < 0)
		goto err;

	ret = dvb_usbv2_adapter_init(d);
	if (ret < 0)
		goto err;

	if (d->props->init) {
		ret = d->props->init(d);
		if (ret < 0)
			goto err;
	}

	ret = dvb_usbv2_remote_init(d);
	if (ret < 0)
		goto err;

	dvb_usbv2_device_power_ctrl(d, 0);

	return 0;
err:
	dvb_usbv2_device_power_ctrl(d, 0);
	dev_dbg(&d->udev->dev, "%s: failed=%d\n", __func__, ret);
	return ret;
}
