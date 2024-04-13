static void ccid3_hc_tx_no_feedback_timer(unsigned long data)
{
	struct sock *sk = (struct sock *)data;
	struct ccid3_hc_tx_sock *hc = ccid3_hc_tx_sk(sk);
	unsigned long t_nfb = USEC_PER_SEC / 5;

	bh_lock_sock(sk);
	if (sock_owned_by_user(sk)) {
		/* Try again later. */
		/* XXX: set some sensible MIB */
		goto restart_timer;
	}

	ccid3_pr_debug("%s(%p, state=%s) - entry\n", dccp_role(sk), sk,
		       ccid3_tx_state_name(hc->tx_state));

	/* Ignore and do not restart after leaving the established state */
	if ((1 << sk->sk_state) & ~(DCCPF_OPEN | DCCPF_PARTOPEN))
		goto out;

	/* Reset feedback state to "no feedback received" */
	if (hc->tx_state == TFRC_SSTATE_FBACK)
		ccid3_hc_tx_set_state(sk, TFRC_SSTATE_NO_FBACK);

	/*
	 * Determine new allowed sending rate X as per draft rfc3448bis-00, 4.4
	 * RTO is 0 if and only if no feedback has been received yet.
	 */
	if (hc->tx_t_rto == 0 || hc->tx_p == 0) {

		/* halve send rate directly */
		hc->tx_x = max(hc->tx_x / 2,
			       (((__u64)hc->tx_s) << 6) / TFRC_T_MBI);
		ccid3_update_send_interval(hc);
	} else {
		/*
		 *  Modify the cached value of X_recv
		 *
		 *  If (X_calc > 2 * X_recv)
		 *    X_recv = max(X_recv / 2, s / (2 * t_mbi));
		 *  Else
		 *    X_recv = X_calc / 4;
		 *
		 *  Note that X_recv is scaled by 2^6 while X_calc is not
		 */
		if (hc->tx_x_calc > (hc->tx_x_recv >> 5))
			hc->tx_x_recv =
				max(hc->tx_x_recv / 2,
				    (((__u64)hc->tx_s) << 6) / (2*TFRC_T_MBI));
		else {
			hc->tx_x_recv = hc->tx_x_calc;
			hc->tx_x_recv <<= 4;
		}
		ccid3_hc_tx_update_x(sk, NULL);
	}
	ccid3_pr_debug("Reduced X to %llu/64 bytes/sec\n",
			(unsigned long long)hc->tx_x);

	/*
	 * Set new timeout for the nofeedback timer.
	 * See comments in packet_recv() regarding the value of t_RTO.
	 */
	if (unlikely(hc->tx_t_rto == 0))	/* no feedback received yet */
		t_nfb = TFRC_INITIAL_TIMEOUT;
	else
		t_nfb = max(hc->tx_t_rto, 2 * hc->tx_t_ipi);

restart_timer:
	sk_reset_timer(sk, &hc->tx_no_feedback_timer,
			   jiffies + usecs_to_jiffies(t_nfb));
out:
	bh_unlock_sock(sk);
	sock_put(sk);
}
