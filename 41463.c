static void tg3_ptp_fini(struct tg3 *tp)
{
	if (!tg3_flag(tp, PTP_CAPABLE) || !tp->ptp_clock)
		return;

	ptp_clock_unregister(tp->ptp_clock);
	tp->ptp_clock = NULL;
	tp->ptp_adjust = 0;
}
