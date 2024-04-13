static u32 ccid3_hc_tx_idle_rtt(struct ccid3_hc_tx_sock *hc, ktime_t now)
{
	u32 delta = ktime_us_delta(now, hc->tx_t_last_win_count);

	return delta / hc->tx_rtt;
}
