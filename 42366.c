static int ccid3_hc_tx_init(struct ccid *ccid, struct sock *sk)
{
	struct ccid3_hc_tx_sock *hc = ccid_priv(ccid);

	hc->tx_state = TFRC_SSTATE_NO_SENT;
	hc->tx_hist  = NULL;
	setup_timer(&hc->tx_no_feedback_timer,
			ccid3_hc_tx_no_feedback_timer, (unsigned long)sk);
	return 0;
}
