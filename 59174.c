u32 dccp_sample_rtt(struct sock *sk, long delta)
{
	/* dccpor_elapsed_time is either zeroed out or set and > 0 */
	delta -= dccp_sk(sk)->dccps_options_received.dccpor_elapsed_time * 10;

	if (unlikely(delta <= 0)) {
		DCCP_WARN("unusable RTT sample %ld, using min\n", delta);
		return DCCP_SANE_RTT_MIN;
	}
	if (unlikely(delta > DCCP_SANE_RTT_MAX)) {
		DCCP_WARN("RTT sample %ld too large, using max\n", delta);
		return DCCP_SANE_RTT_MAX;
	}

	return delta;
}
