static void ccid3_hc_rx_exit(struct sock *sk)
{
	struct ccid3_hc_rx_sock *hc = ccid3_hc_rx_sk(sk);

	tfrc_rx_hist_purge(&hc->rx_hist);
	tfrc_lh_cleanup(&hc->rx_li_hist);
}
