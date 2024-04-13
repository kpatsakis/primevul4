static void tg3_ptp_init(struct tg3 *tp)
{
	if (!tg3_flag(tp, PTP_CAPABLE))
		return;

	/* Initialize the hardware clock to the system time. */
	tg3_refclk_write(tp, ktime_to_ns(ktime_get_real()));
	tp->ptp_adjust = 0;
	tp->ptp_info = tg3_ptp_caps;
}
