static int cxusb_d680_dmb_tuner_attach(struct dvb_usb_adapter *adap)
{
	struct dvb_frontend *fe;
	fe = dvb_attach(mxl5005s_attach, adap->fe_adap[0].fe,
			&adap->dev->i2c_adap, &d680_dmb_tuner);
	return (fe == NULL) ? -EIO : 0;
}
