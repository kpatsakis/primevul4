static int dvb_usbv2_i2c_exit(struct dvb_usb_device *d)
{
	dev_dbg(&d->udev->dev, "%s:\n", __func__);

	if (d->i2c_adap.algo)
		i2c_del_adapter(&d->i2c_adap);

	return 0;
}
