static int ttusbdecfe_dvbt_get_tune_settings(struct dvb_frontend* fe,
					struct dvb_frontend_tune_settings* fesettings)
{
		fesettings->min_delay_ms = 1500;
		/* Drift compensation makes no sense for DVB-T */
		fesettings->step_size = 0;
		fesettings->max_drift = 0;
		return 0;
}
