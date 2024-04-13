static int cxusb_dvico_xc3028_tuner_attach(struct dvb_usb_adapter *adap)
{
	struct dvb_frontend	 *fe;
	struct xc2028_config	  cfg = {
		.i2c_adap  = &adap->dev->i2c_adap,
		.i2c_addr  = 0x61,
	};
	static struct xc2028_ctrl ctl = {
		.fname       = XC2028_DEFAULT_FIRMWARE,
		.max_len     = 64,
		.demod       = XC3028_FE_ZARLINK456,
	};

	/* FIXME: generalize & move to common area */
	adap->fe_adap[0].fe->callback = dvico_bluebird_xc2028_callback;

	fe = dvb_attach(xc2028_attach, adap->fe_adap[0].fe, &cfg);
	if (fe == NULL || fe->ops.tuner_ops.set_config == NULL)
		return -EIO;

	fe->ops.tuner_ops.set_config(fe, &ctl);

	return 0;
}
