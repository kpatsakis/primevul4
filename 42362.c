static void ccid3_hc_rx_set_state(struct sock *sk,
				  enum ccid3_hc_rx_states state)
{
	struct ccid3_hc_rx_sock *hc = ccid3_hc_rx_sk(sk);
	enum ccid3_hc_rx_states oldstate = hc->rx_state;

	ccid3_pr_debug("%s(%p) %-8.8s -> %s\n",
		       dccp_role(sk), sk, ccid3_rx_state_name(oldstate),
		       ccid3_rx_state_name(state));
	WARN_ON(state == oldstate);
	hc->rx_state = state;
}
