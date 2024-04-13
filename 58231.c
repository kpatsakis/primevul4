static int dib7070_set_param_override(struct dvb_frontend *fe)
{
	struct dtv_frontend_properties *p = &fe->dtv_property_cache;
	struct dvb_usb_adapter *adap = fe->dvb->priv;
	struct dib0700_adapter_state *state = adap->priv;

	u16 offset;
	u8 band = BAND_OF_FREQUENCY(p->frequency/1000);
	switch (band) {
	case BAND_VHF: offset = 950; break;
	default:
	case BAND_UHF: offset = 550; break;
	}

	state->dib7000p_ops.set_wbd_ref(fe, offset + dib0070_wbd_offset(fe));

	return state->set_param_save(fe);
}
