static void ccid3_hc_tx_exit(struct sock *sk)
{
	struct ccid3_hc_tx_sock *hc = ccid3_hc_tx_sk(sk);

	sk_stop_timer(sk, &hc->tx_no_feedback_timer);
	tfrc_tx_hist_purge(&hc->tx_hist);
}
