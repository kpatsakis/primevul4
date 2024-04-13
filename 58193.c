static int cxusb_cx22702_frontend_attach(struct dvb_usb_adapter *adap)
{
	u8 b;
	if (usb_set_interface(adap->dev->udev, 0, 6) < 0)
		err("set interface failed");

	cxusb_ctrl_msg(adap->dev, CMD_DIGITAL, NULL, 0, &b, 1);

	adap->fe_adap[0].fe = dvb_attach(cx22702_attach, &cxusb_cx22702_config,
					 &adap->dev->i2c_adap);
	if ((adap->fe_adap[0].fe) != NULL)
		return 0;

	return -EIO;
}
