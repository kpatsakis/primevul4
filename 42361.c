static void ccid3_hc_rx_send_feedback(struct sock *sk,
				      const struct sk_buff *skb,
				      enum ccid3_fback_type fbtype)
{
	struct ccid3_hc_rx_sock *hc = ccid3_hc_rx_sk(sk);
	struct dccp_sock *dp = dccp_sk(sk);
	ktime_t now = ktime_get_real();
	s64 delta = 0;

	switch (fbtype) {
	case CCID3_FBACK_INITIAL:
		hc->rx_x_recv = 0;
		hc->rx_pinv   = ~0U;   /* see RFC 4342, 8.5 */
		break;
	case CCID3_FBACK_PARAM_CHANGE:
		/*
		 * When parameters change (new loss or p > p_prev), we do not
		 * have a reliable estimate for R_m of [RFC 3448, 6.2] and so
		 * need to  reuse the previous value of X_recv. However, when
		 * X_recv was 0 (due to early loss), this would kill X down to
		 * s/t_mbi (i.e. one packet in 64 seconds).
		 * To avoid such drastic reduction, we approximate X_recv as
		 * the number of bytes since last feedback.
		 * This is a safe fallback, since X is bounded above by X_calc.
		 */
		if (hc->rx_x_recv > 0)
			break;
		/* fall through */
	case CCID3_FBACK_PERIODIC:
		delta = ktime_us_delta(now, hc->rx_tstamp_last_feedback);
		if (delta <= 0)
			DCCP_BUG("delta (%ld) <= 0", (long)delta);
		else
			hc->rx_x_recv = scaled_div32(hc->rx_bytes_recv, delta);
		break;
	default:
		return;
	}

	ccid3_pr_debug("Interval %ldusec, X_recv=%u, 1/p=%u\n", (long)delta,
		       hc->rx_x_recv, hc->rx_pinv);

	hc->rx_tstamp_last_feedback = now;
	hc->rx_last_counter	    = dccp_hdr(skb)->dccph_ccval;
	hc->rx_bytes_recv	    = 0;

	dp->dccps_hc_rx_insert_options = 1;
	dccp_send_ack(sk);
}
