static inline u64 rfc3390_initial_rate(struct sock *sk)
{
	const struct ccid3_hc_tx_sock *hc = ccid3_hc_tx_sk(sk);
	const __u32 w_init = clamp_t(__u32, 4380U, 2 * hc->tx_s, 4 * hc->tx_s);

	return scaled_div(w_init << 6, hc->tx_rtt);
}
