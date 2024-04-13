void ax25_fillin_cb(ax25_cb *ax25, ax25_dev *ax25_dev)
{
	ax25->ax25_dev = ax25_dev;

	if (ax25->ax25_dev != NULL) {
		ax25_fillin_cb_from_dev(ax25, ax25_dev);
		return;
	}

	/*
	 * No device, use kernel / AX.25 spec default values
	 */
	ax25->rtt     = msecs_to_jiffies(AX25_DEF_T1) / 2;
	ax25->t1      = msecs_to_jiffies(AX25_DEF_T1);
	ax25->t2      = msecs_to_jiffies(AX25_DEF_T2);
	ax25->t3      = msecs_to_jiffies(AX25_DEF_T3);
	ax25->n2      = AX25_DEF_N2;
	ax25->paclen  = AX25_DEF_PACLEN;
	ax25->idle    = msecs_to_jiffies(AX25_DEF_IDLE);
	ax25->backoff = AX25_DEF_BACKOFF;

	if (AX25_DEF_AXDEFMODE) {
		ax25->modulus = AX25_EMODULUS;
		ax25->window  = AX25_DEF_EWINDOW;
	} else {
		ax25->modulus = AX25_MODULUS;
		ax25->window  = AX25_DEF_WINDOW;
	}
}
