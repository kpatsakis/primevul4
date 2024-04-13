static int ttusbdecfe_dvbs_set_frontend(struct dvb_frontend *fe)
{
	struct dtv_frontend_properties *p = &fe->dtv_property_cache;
	struct ttusbdecfe_state* state = (struct ttusbdecfe_state*) fe->demodulator_priv;

	u8 b[] = { 0x00, 0x00, 0x00, 0x01,
		   0x00, 0x00, 0x00, 0x00,
		   0x00, 0x00, 0x00, 0x01,
		   0x00, 0x00, 0x00, 0x00,
		   0x00, 0x00, 0x00, 0x00,
		   0x00, 0x00, 0x00, 0x00,
		   0x00, 0x00, 0x00, 0x00,
		   0x00, 0x00, 0x00, 0x00,
		   0x00, 0x00, 0x00, 0x00,
		   0x00, 0x00, 0x00, 0x00 };
	__be32 freq;
	__be32 sym_rate;
	__be32 band;
	__be32 lnb_voltage;

	freq = htonl(p->frequency +
	       (state->hi_band ? LOF_HI : LOF_LO));
	memcpy(&b[4], &freq, sizeof(u32));
	sym_rate = htonl(p->symbol_rate);
	memcpy(&b[12], &sym_rate, sizeof(u32));
	band = htonl(state->hi_band ? LOF_HI : LOF_LO);
	memcpy(&b[24], &band, sizeof(u32));
	lnb_voltage = htonl(state->voltage);
	memcpy(&b[28], &lnb_voltage, sizeof(u32));

	state->config->send_command(fe, 0x71, sizeof(b), b, NULL, NULL);

	return 0;
}
