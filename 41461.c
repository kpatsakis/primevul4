static int tg3_ptp_adjtime(struct ptp_clock_info *ptp, s64 delta)
{
	struct tg3 *tp = container_of(ptp, struct tg3, ptp_info);

	tg3_full_lock(tp, 0);
	tp->ptp_adjust += delta;
	tg3_full_unlock(tp);

	return 0;
}
