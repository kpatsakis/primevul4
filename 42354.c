static u32 ccid3_first_li(struct sock *sk)
{
	struct ccid3_hc_rx_sock *hc = ccid3_hc_rx_sk(sk);
	u32 x_recv, p, delta;
	u64 fval;

	if (hc->rx_rtt == 0) {
		DCCP_WARN("No RTT estimate available, using fallback RTT\n");
		hc->rx_rtt = DCCP_FALLBACK_RTT;
	}

	delta  = ktime_to_us(net_timedelta(hc->rx_tstamp_last_feedback));
	x_recv = scaled_div32(hc->rx_bytes_recv, delta);
	if (x_recv == 0) {		/* would also trigger divide-by-zero */
		DCCP_WARN("X_recv==0\n");
		if (hc->rx_x_recv == 0) {
			DCCP_BUG("stored value of X_recv is zero");
			return ~0U;
		}
		x_recv = hc->rx_x_recv;
	}

	fval = scaled_div(hc->rx_s, hc->rx_rtt);
	fval = scaled_div32(fval, x_recv);
	p = tfrc_calc_x_reverse_lookup(fval);

	ccid3_pr_debug("%s(%p), receive rate=%u bytes/s, implied "
		       "loss rate=%u\n", dccp_role(sk), sk, x_recv, p);

	return p == 0 ? ~0U : scaled_div(1, p);
}
