static void ccid3_hc_tx_packet_recv(struct sock *sk, struct sk_buff *skb)
{
	struct ccid3_hc_tx_sock *hc = ccid3_hc_tx_sk(sk);
	struct tfrc_tx_hist_entry *acked;
	ktime_t now;
	unsigned long t_nfb;
	u32 r_sample;

	/* we are only interested in ACKs */
	if (!(DCCP_SKB_CB(skb)->dccpd_type == DCCP_PKT_ACK ||
	      DCCP_SKB_CB(skb)->dccpd_type == DCCP_PKT_DATAACK))
		return;
	/*
	 * Locate the acknowledged packet in the TX history.
	 *
	 * Returning "entry not found" here can for instance happen when
	 *  - the host has not sent out anything (e.g. a passive server),
	 *  - the Ack is outdated (packet with higher Ack number was received),
	 *  - it is a bogus Ack (for a packet not sent on this connection).
	 */
	acked = tfrc_tx_hist_find_entry(hc->tx_hist, dccp_hdr_ack_seq(skb));
	if (acked == NULL)
		return;
	/* For the sake of RTT sampling, ignore/remove all older entries */
	tfrc_tx_hist_purge(&acked->next);

	/* Update the moving average for the RTT estimate (RFC 3448, 4.3) */
	now	  = ktime_get_real();
	r_sample  = dccp_sample_rtt(sk, ktime_us_delta(now, acked->stamp));
	hc->tx_rtt = tfrc_ewma(hc->tx_rtt, r_sample, 9);

	/*
	 * Update allowed sending rate X as per draft rfc3448bis-00, 4.2/3
	 */
	if (hc->tx_state == TFRC_SSTATE_NO_FBACK) {
		ccid3_hc_tx_set_state(sk, TFRC_SSTATE_FBACK);

		if (hc->tx_t_rto == 0) {
			/*
			 * Initial feedback packet: Larger Initial Windows (4.2)
			 */
			hc->tx_x    = rfc3390_initial_rate(sk);
			hc->tx_t_ld = now;

			ccid3_update_send_interval(hc);

			goto done_computing_x;
		} else if (hc->tx_p == 0) {
			/*
			 * First feedback after nofeedback timer expiry (4.3)
			 */
			goto done_computing_x;
		}
	}

	/* Update sending rate (step 4 of [RFC 3448, 4.3]) */
	if (hc->tx_p > 0)
		hc->tx_x_calc = tfrc_calc_x(hc->tx_s, hc->tx_rtt, hc->tx_p);
	ccid3_hc_tx_update_x(sk, &now);

done_computing_x:
	ccid3_pr_debug("%s(%p), RTT=%uus (sample=%uus), s=%u, "
			       "p=%u, X_calc=%u, X_recv=%u, X=%u\n",
			       dccp_role(sk), sk, hc->tx_rtt, r_sample,
			       hc->tx_s, hc->tx_p, hc->tx_x_calc,
			       (unsigned int)(hc->tx_x_recv >> 6),
			       (unsigned int)(hc->tx_x >> 6));

	/* unschedule no feedback timer */
	sk_stop_timer(sk, &hc->tx_no_feedback_timer);

	/*
	 * As we have calculated new ipi, delta, t_nom it is possible
	 * that we now can send a packet, so wake up dccp_wait_for_ccid
	 */
	sk->sk_write_space(sk);

	/*
	 * Update timeout interval for the nofeedback timer. In order to control
	 * rate halving on networks with very low RTTs (<= 1 ms), use per-route
	 * tunable RTAX_RTO_MIN value as the lower bound.
	 */
	hc->tx_t_rto = max_t(u32, 4 * hc->tx_rtt,
				  USEC_PER_SEC/HZ * tcp_rto_min(sk));
	/*
	 * Schedule no feedback timer to expire in
	 * max(t_RTO, 2 * s/X)  =  max(t_RTO, 2 * t_ipi)
	 */
	t_nfb = max(hc->tx_t_rto, 2 * hc->tx_t_ipi);

	ccid3_pr_debug("%s(%p), Scheduled no feedback timer to "
		       "expire in %lu jiffies (%luus)\n",
		       dccp_role(sk), sk, usecs_to_jiffies(t_nfb), t_nfb);

	sk_reset_timer(sk, &hc->tx_no_feedback_timer,
			   jiffies + usecs_to_jiffies(t_nfb));
}
