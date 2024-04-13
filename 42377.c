static void ccid3_update_send_interval(struct ccid3_hc_tx_sock *hc)
{
	hc->tx_t_ipi = scaled_div32(((u64)hc->tx_s) << 6, hc->tx_x);

	DCCP_BUG_ON(hc->tx_t_ipi == 0);
	ccid3_pr_debug("t_ipi=%u, s=%u, X=%u\n", hc->tx_t_ipi,
		       hc->tx_s, (unsigned int)(hc->tx_x >> 6));
}
