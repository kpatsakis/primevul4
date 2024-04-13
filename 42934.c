struct dvb_frontend* ttusbdecfe_dvbt_attach(const struct ttusbdecfe_config* config)
{
	struct ttusbdecfe_state* state = NULL;

	/* allocate memory for the internal state */
	state = kmalloc(sizeof(struct ttusbdecfe_state), GFP_KERNEL);
	if (state == NULL)
		return NULL;

	/* setup the state */
	state->config = config;

	/* create dvb_frontend */
	memcpy(&state->frontend.ops, &ttusbdecfe_dvbt_ops, sizeof(struct dvb_frontend_ops));
	state->frontend.demodulator_priv = state;
	return &state->frontend;
}
