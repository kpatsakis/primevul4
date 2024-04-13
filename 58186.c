static int cxusb_aver_lgdt3303_frontend_attach(struct dvb_usb_adapter *adap)
{
	adap->fe_adap[0].fe = dvb_attach(lgdt330x_attach, &cxusb_aver_lgdt3303_config,
			      &adap->dev->i2c_adap);
	if (adap->fe_adap[0].fe != NULL)
		return 0;

	return -EIO;
}
