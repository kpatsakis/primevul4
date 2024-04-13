static int ttusbdecfe_dvbs_set_tone(struct dvb_frontend* fe, fe_sec_tone_mode_t tone)
{
	struct ttusbdecfe_state* state = (struct ttusbdecfe_state*) fe->demodulator_priv;

	state->hi_band = (SEC_TONE_ON == tone);

	return 0;
}
