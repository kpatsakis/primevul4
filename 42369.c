static void ccid3_hc_tx_packet_sent(struct sock *sk, unsigned int len)
{
	struct ccid3_hc_tx_sock *hc = ccid3_hc_tx_sk(sk);

	ccid3_hc_tx_update_s(hc, len);

	if (tfrc_tx_hist_add(&hc->tx_hist, dccp_sk(sk)->dccps_gss))
		DCCP_CRIT("packet history - out of memory!");
}
