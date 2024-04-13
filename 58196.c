static int cxusb_d680_dmb_frontend_attach(struct dvb_usb_adapter *adap)
{
	struct dvb_usb_device *d = adap->dev;
	int n;

	/* Select required USB configuration */
	if (usb_set_interface(d->udev, 0, 0) < 0)
		err("set interface failed");

	/* Unblock all USB pipes */
	usb_clear_halt(d->udev,
		usb_sndbulkpipe(d->udev, d->props.generic_bulk_ctrl_endpoint));
	usb_clear_halt(d->udev,
		usb_rcvbulkpipe(d->udev, d->props.generic_bulk_ctrl_endpoint));
	usb_clear_halt(d->udev,
		usb_rcvbulkpipe(d->udev, d->props.adapter[0].fe[0].stream.endpoint));

	/* Drain USB pipes to avoid hang after reboot */
	for (n = 0;  n < 5;  n++) {
		cxusb_d680_dmb_drain_message(d);
		cxusb_d680_dmb_drain_video(d);
		msleep(200);
	}

	/* Reset the tuner */
	if (cxusb_d680_dmb_gpio_tuner(d, 0x07, 0) < 0) {
		err("clear tuner gpio failed");
		return -EIO;
	}
	msleep(100);
	if (cxusb_d680_dmb_gpio_tuner(d, 0x07, 1) < 0) {
		err("set tuner gpio failed");
		return -EIO;
	}
	msleep(100);

	/* Attach frontend */
	adap->fe_adap[0].fe = dvb_attach(lgs8gxx_attach, &d680_lgs8gl5_cfg, &d->i2c_adap);
	if (adap->fe_adap[0].fe == NULL)
		return -EIO;

	return 0;
}
