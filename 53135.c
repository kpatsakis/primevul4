static void tcp_fastretrans_alert(struct sock *sk, const int acked,
				  const int prior_unsacked,
				  bool is_dupack, int flag)
{
	struct inet_connection_sock *icsk = inet_csk(sk);
	struct tcp_sock *tp = tcp_sk(sk);
	bool do_lost = is_dupack || ((flag & FLAG_DATA_SACKED) &&
				    (tcp_fackets_out(tp) > tp->reordering));
	int fast_rexmit = 0;

	if (WARN_ON(!tp->packets_out && tp->sacked_out))
		tp->sacked_out = 0;
	if (WARN_ON(!tp->sacked_out && tp->fackets_out))
		tp->fackets_out = 0;

	/* Now state machine starts.
	 * A. ECE, hence prohibit cwnd undoing, the reduction is required. */
	if (flag & FLAG_ECE)
		tp->prior_ssthresh = 0;

	/* B. In all the states check for reneging SACKs. */
	if (tcp_check_sack_reneging(sk, flag))
		return;

	/* C. Check consistency of the current state. */
	tcp_verify_left_out(tp);

	/* D. Check state exit conditions. State can be terminated
	 *    when high_seq is ACKed. */
	if (icsk->icsk_ca_state == TCP_CA_Open) {
		WARN_ON(tp->retrans_out != 0);
		tp->retrans_stamp = 0;
	} else if (!before(tp->snd_una, tp->high_seq)) {
		switch (icsk->icsk_ca_state) {
		case TCP_CA_CWR:
			/* CWR is to be held something *above* high_seq
			 * is ACKed for CWR bit to reach receiver. */
			if (tp->snd_una != tp->high_seq) {
				tcp_end_cwnd_reduction(sk);
				tcp_set_ca_state(sk, TCP_CA_Open);
			}
			break;

		case TCP_CA_Recovery:
			if (tcp_is_reno(tp))
				tcp_reset_reno_sack(tp);
			if (tcp_try_undo_recovery(sk))
				return;
			tcp_end_cwnd_reduction(sk);
			break;
		}
	}

	/* Use RACK to detect loss */
	if (sysctl_tcp_recovery & TCP_RACK_LOST_RETRANS &&
	    tcp_rack_mark_lost(sk))
		flag |= FLAG_LOST_RETRANS;

	/* E. Process state. */
	switch (icsk->icsk_ca_state) {
	case TCP_CA_Recovery:
		if (!(flag & FLAG_SND_UNA_ADVANCED)) {
			if (tcp_is_reno(tp) && is_dupack)
				tcp_add_reno_sack(sk);
		} else {
			if (tcp_try_undo_partial(sk, acked, prior_unsacked, flag))
				return;
			/* Partial ACK arrived. Force fast retransmit. */
			do_lost = tcp_is_reno(tp) ||
				  tcp_fackets_out(tp) > tp->reordering;
		}
		if (tcp_try_undo_dsack(sk)) {
			tcp_try_keep_open(sk);
			return;
		}
		break;
	case TCP_CA_Loss:
		tcp_process_loss(sk, flag, is_dupack);
		if (icsk->icsk_ca_state != TCP_CA_Open &&
		    !(flag & FLAG_LOST_RETRANS))
			return;
		/* Change state if cwnd is undone or retransmits are lost */
	default:
		if (tcp_is_reno(tp)) {
			if (flag & FLAG_SND_UNA_ADVANCED)
				tcp_reset_reno_sack(tp);
			if (is_dupack)
				tcp_add_reno_sack(sk);
		}

		if (icsk->icsk_ca_state <= TCP_CA_Disorder)
			tcp_try_undo_dsack(sk);

		if (!tcp_time_to_recover(sk, flag)) {
			tcp_try_to_open(sk, flag, prior_unsacked);
			return;
		}

		/* MTU probe failure: don't reduce cwnd */
		if (icsk->icsk_ca_state < TCP_CA_CWR &&
		    icsk->icsk_mtup.probe_size &&
		    tp->snd_una == tp->mtu_probe.probe_seq_start) {
			tcp_mtup_probe_failed(sk);
			/* Restores the reduction we did in tcp_mtup_probe() */
			tp->snd_cwnd++;
			tcp_simple_retransmit(sk);
			return;
		}

		/* Otherwise enter Recovery state */
		tcp_enter_recovery(sk, (flag & FLAG_ECE));
		fast_rexmit = 1;
	}

	if (do_lost)
		tcp_update_scoreboard(sk, fast_rexmit);
	tcp_cwnd_reduction(sk, prior_unsacked, fast_rexmit, flag);
	tcp_xmit_retransmit_queue(sk);
}
