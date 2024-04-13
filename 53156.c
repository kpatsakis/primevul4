static bool tcp_time_to_recover(struct sock *sk, int flag)
{
	struct tcp_sock *tp = tcp_sk(sk);
	__u32 packets_out;

	/* Trick#1: The loss is proven. */
	if (tp->lost_out)
		return true;

	/* Not-A-Trick#2 : Classic rule... */
	if (tcp_dupack_heuristics(tp) > tp->reordering)
		return true;

	/* Trick#4: It is still not OK... But will it be useful to delay
	 * recovery more?
	 */
	packets_out = tp->packets_out;
	if (packets_out <= tp->reordering &&
	    tp->sacked_out >= max_t(__u32, packets_out/2, sysctl_tcp_reordering) &&
	    !tcp_may_send_now(sk)) {
		/* We have nothing to send. This connection is limited
		 * either by receiver window or by application.
		 */
		return true;
	}

	/* If a thin stream is detected, retransmit after first
	 * received dupack. Employ only if SACK is supported in order
	 * to avoid possible corner-case series of spurious retransmissions
	 * Use only if there are no unsent data.
	 */
	if ((tp->thin_dupack || sysctl_tcp_thin_dupack) &&
	    tcp_stream_is_thin(tp) && tcp_dupack_heuristics(tp) > 1 &&
	    tcp_is_sack(tp) && !tcp_send_head(sk))
		return true;

	/* Trick#6: TCP early retransmit, per RFC5827.  To avoid spurious
	 * retransmissions due to small network reorderings, we implement
	 * Mitigation A.3 in the RFC and delay the retransmission for a short
	 * interval if appropriate.
	 */
	if (tp->do_early_retrans && !tp->retrans_out && tp->sacked_out &&
	    (tp->packets_out >= (tp->sacked_out + 1) && tp->packets_out < 4) &&
	    !tcp_may_send_now(sk))
		return !tcp_pause_early_retransmit(sk, flag);

	return false;
}
