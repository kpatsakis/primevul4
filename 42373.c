static inline void ccid3_hc_tx_update_s(struct ccid3_hc_tx_sock *hc, int len)
{
	const u16 old_s = hc->tx_s;

	hc->tx_s = tfrc_ewma(hc->tx_s, len, 9);

	if (hc->tx_s != old_s)
		ccid3_update_send_interval(hc);
}
