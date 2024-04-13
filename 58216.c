static int cxusb_lgh064f_tuner_attach(struct dvb_usb_adapter *adap)
{
	dvb_attach(simple_tuner_attach, adap->fe_adap[0].fe,
		   &adap->dev->i2c_adap, 0x61, TUNER_LG_TDVS_H06XF);
	return 0;
}
