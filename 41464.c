static int tg3_ptp_gettime(struct ptp_clock_info *ptp, struct timespec *ts)
{
	u64 ns;
	u32 remainder;
	struct tg3 *tp = container_of(ptp, struct tg3, ptp_info);

	tg3_full_lock(tp, 0);
	ns = tg3_refclk_read(tp);
	ns += tp->ptp_adjust;
	tg3_full_unlock(tp);

	ts->tv_sec = div_u64_rem(ns, 1000000000, &remainder);
	ts->tv_nsec = remainder;

	return 0;
}
