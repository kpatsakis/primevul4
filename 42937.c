static int ttusbdecfe_dvbt_set_frontend(struct dvb_frontend *fe)
{
	struct dtv_frontend_properties *p = &fe->dtv_property_cache;
	struct ttusbdecfe_state* state = (struct ttusbdecfe_state*) fe->demodulator_priv;
	u8 b[] = { 0x00, 0x00, 0x00, 0x03,
		   0x00, 0x00, 0x00, 0x00,
		   0x00, 0x00, 0x00, 0x01,
		   0x00, 0x00, 0x00, 0xff,
		   0x00, 0x00, 0x00, 0xff };

	__be32 freq = htonl(p->frequency / 1000);
	memcpy(&b[4], &freq, sizeof (u32));
	state->config->send_command(fe, 0x71, sizeof(b), b, NULL, NULL);

	return 0;
}
