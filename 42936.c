static int ttusbdecfe_dvbt_read_status(struct dvb_frontend *fe,
	fe_status_t *status)
{
	struct ttusbdecfe_state* state = fe->demodulator_priv;
	u8 b[] = { 0x00, 0x00, 0x00, 0x00,
		   0x00, 0x00, 0x00, 0x00 };
	u8 result[4];
	int len, ret;

	*status=0;

	ret=state->config->send_command(fe, 0x73, sizeof(b), b, &len, result);
	if(ret)
		return ret;

	if(len != 4) {
		printk(KERN_ERR "%s: unexpected reply\n", __func__);
		return -EIO;
	}

	switch(result[3]) {
		case 1:  /* not tuned yet */
		case 2:  /* no signal/no lock*/
			break;
		case 3:	 /* signal found and locked*/
			*status = FE_HAS_SIGNAL | FE_HAS_VITERBI |
			FE_HAS_SYNC | FE_HAS_CARRIER | FE_HAS_LOCK;
			break;
		case 4:
			*status = FE_TIMEDOUT;
			break;
		default:
			pr_info("%s: returned unknown value: %d\n",
				__func__, result[3]);
			return -EIO;
	}

	return 0;
}
