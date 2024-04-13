static int cxusb_fmd1216me_tuner_attach(struct dvb_usb_adapter *adap)
{
	dvb_attach(simple_tuner_attach, adap->fe_adap[0].fe,
		   &adap->dev->i2c_adap, 0x61,
		   TUNER_PHILIPS_FMD1216ME_MK3);
	return 0;
}
