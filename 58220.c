static int cxusb_mxl5003s_tuner_attach(struct dvb_usb_adapter *adap)
{
	dvb_attach(mxl5005s_attach, adap->fe_adap[0].fe,
		   &adap->dev->i2c_adap, &aver_a868r_tuner);
	return 0;
}
