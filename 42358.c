static int ccid3_hc_rx_init(struct ccid *ccid, struct sock *sk)
{
	struct ccid3_hc_rx_sock *hc = ccid_priv(ccid);

	hc->rx_state = TFRC_RSTATE_NO_DATA;
	tfrc_lh_init(&hc->rx_li_hist);
	return tfrc_rx_hist_alloc(&hc->rx_hist);
}
