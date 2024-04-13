static void tcp_update_rtt_min(struct sock *sk, u32 rtt_us)
{
	const u32 now = tcp_time_stamp, wlen = sysctl_tcp_min_rtt_wlen * HZ;
	struct rtt_meas *m = tcp_sk(sk)->rtt_min;
	struct rtt_meas rttm = { .rtt = (rtt_us ? : 1), .ts = now };
	u32 elapsed;

	/* Check if the new measurement updates the 1st, 2nd, or 3rd choices */
	if (unlikely(rttm.rtt <= m[0].rtt))
		m[0] = m[1] = m[2] = rttm;
	else if (rttm.rtt <= m[1].rtt)
		m[1] = m[2] = rttm;
	else if (rttm.rtt <= m[2].rtt)
		m[2] = rttm;

	elapsed = now - m[0].ts;
	if (unlikely(elapsed > wlen)) {
		/* Passed entire window without a new min so make 2nd choice
		 * the new min & 3rd choice the new 2nd. So forth and so on.
		 */
		m[0] = m[1];
		m[1] = m[2];
		m[2] = rttm;
		if (now - m[0].ts > wlen) {
			m[0] = m[1];
			m[1] = rttm;
			if (now - m[0].ts > wlen)
				m[0] = rttm;
		}
	} else if (m[1].ts == m[0].ts && elapsed > wlen / 4) {
		/* Passed a quarter of the window without a new min so
		 * take 2nd choice from the 2nd quarter of the window.
		 */
		m[2] = m[1] = rttm;
	} else if (m[2].ts == m[1].ts && elapsed > wlen / 2) {
		/* Passed half the window without a new min so take the 3rd
		 * choice from the last half of the window.
		 */
		m[2] = rttm;
	}
}
