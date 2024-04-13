static void ax25_fillin_cb_from_dev(ax25_cb *ax25, ax25_dev *ax25_dev)
{
	ax25->rtt     = msecs_to_jiffies(ax25_dev->values[AX25_VALUES_T1]) / 2;
	ax25->t1      = msecs_to_jiffies(ax25_dev->values[AX25_VALUES_T1]);
	ax25->t2      = msecs_to_jiffies(ax25_dev->values[AX25_VALUES_T2]);
	ax25->t3      = msecs_to_jiffies(ax25_dev->values[AX25_VALUES_T3]);
	ax25->n2      = ax25_dev->values[AX25_VALUES_N2];
	ax25->paclen  = ax25_dev->values[AX25_VALUES_PACLEN];
	ax25->idle    = msecs_to_jiffies(ax25_dev->values[AX25_VALUES_IDLE]);
	ax25->backoff = ax25_dev->values[AX25_VALUES_BACKOFF];

	if (ax25_dev->values[AX25_VALUES_AXDEFMODE]) {
		ax25->modulus = AX25_EMODULUS;
		ax25->window  = ax25_dev->values[AX25_VALUES_EWINDOW];
	} else {
		ax25->modulus = AX25_MODULUS;
		ax25->window  = ax25_dev->values[AX25_VALUES_WINDOW];
	}
}
