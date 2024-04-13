static int ttusbdecfe_dvbs_read_status(struct dvb_frontend *fe,
	fe_status_t *status)
{
	*status = FE_HAS_SIGNAL | FE_HAS_VITERBI |
		FE_HAS_SYNC | FE_HAS_CARRIER | FE_HAS_LOCK;
	return 0;
}
