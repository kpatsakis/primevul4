static int cxusb_nano2_frontend_attach(struct dvb_usb_adapter *adap)
{
	if (usb_set_interface(adap->dev->udev, 0, 1) < 0)
		err("set interface failed");

	cxusb_ctrl_msg(adap->dev, CMD_DIGITAL, NULL, 0, NULL, 0);

	/* reset the tuner and demodulator */
	cxusb_bluebird_gpio_rw(adap->dev, 0x04, 0);
	cxusb_bluebird_gpio_pulse(adap->dev, 0x01, 1);
	cxusb_bluebird_gpio_pulse(adap->dev, 0x02, 1);

	adap->fe_adap[0].fe = dvb_attach(zl10353_attach,
					 &cxusb_zl10353_xc3028_config,
					 &adap->dev->i2c_adap);
	if ((adap->fe_adap[0].fe) != NULL)
		return 0;

	adap->fe_adap[0].fe = dvb_attach(mt352_attach,
					 &cxusb_mt352_xc3028_config,
					 &adap->dev->i2c_adap);
	if ((adap->fe_adap[0].fe) != NULL)
		return 0;

	return -EIO;
}
