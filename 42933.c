static int ttusbdecfe_dvbs_set_voltage(struct dvb_frontend* fe, fe_sec_voltage_t voltage)
{
	struct ttusbdecfe_state* state = (struct ttusbdecfe_state*) fe->demodulator_priv;

	switch (voltage) {
	case SEC_VOLTAGE_13:
		state->voltage = 13;
		break;
	case SEC_VOLTAGE_18:
		state->voltage = 18;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}
