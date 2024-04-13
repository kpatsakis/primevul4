static int cxusb_dualdig4_rev2_tuner_attach(struct dvb_usb_adapter *adap)
{
	struct dib0700_adapter_state *st = adap->priv;
	struct i2c_adapter *tun_i2c;

	/*
	 * No need to call dvb7000p_attach here, as it was called
	 * already, as frontend_attach method is called first, and
	 * tuner_attach is only called on sucess.
	 */
	tun_i2c = st->dib7000p_ops.get_i2c_master(adap->fe_adap[0].fe,
					DIBX000_I2C_INTERFACE_TUNER, 1);

	if (dvb_attach(dib0070_attach, adap->fe_adap[0].fe, tun_i2c,
	    &dib7070p_dib0070_config) == NULL)
		return -ENODEV;

	st->set_param_save = adap->fe_adap[0].fe->ops.tuner_ops.set_params;
	adap->fe_adap[0].fe->ops.tuner_ops.set_params = dib7070_set_param_override;
	return 0;
}
