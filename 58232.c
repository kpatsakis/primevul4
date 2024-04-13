static int dib7070_tuner_reset(struct dvb_frontend *fe, int onoff)
{
	struct dvb_usb_adapter *adap = fe->dvb->priv;
	struct dib0700_adapter_state *state = adap->priv;

	return state->dib7000p_ops.set_gpio(fe, 8, 0, !onoff);
}
