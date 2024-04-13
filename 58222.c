static int cxusb_mygica_d689_tuner_attach(struct dvb_usb_adapter *adap)
{
	struct dvb_frontend *fe;
	fe = dvb_attach(max2165_attach, adap->fe_adap[0].fe,
			&adap->dev->i2c_adap, &mygica_d689_max2165_cfg);
	return (fe == NULL) ? -EIO : 0;
}
