static inline void ccid3_hc_tx_update_win_count(struct ccid3_hc_tx_sock *hc,
						ktime_t now)
{
	u32 delta = ktime_us_delta(now, hc->tx_t_last_win_count),
	    quarter_rtts = (4 * delta) / hc->tx_rtt;

	if (quarter_rtts > 0) {
		hc->tx_t_last_win_count = now;
		hc->tx_last_win_count  += min(quarter_rtts, 5U);
		hc->tx_last_win_count  &= 0xF;		/* mod 16 */
	}
}
