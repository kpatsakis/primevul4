static int dccp_rcv_request_sent_state_process(struct sock *sk,
					       struct sk_buff *skb,
					       const struct dccp_hdr *dh,
					       const unsigned int len)
{
	/*
	 *  Step 4: Prepare sequence numbers in REQUEST
	 *     If S.state == REQUEST,
	 *	  If (P.type == Response or P.type == Reset)
	 *		and S.AWL <= P.ackno <= S.AWH,
	 *	     / * Set sequence number variables corresponding to the
	 *		other endpoint, so P will pass the tests in Step 6 * /
	 *	     Set S.GSR, S.ISR, S.SWL, S.SWH
	 *	     / * Response processing continues in Step 10; Reset
	 *		processing continues in Step 9 * /
	*/
	if (dh->dccph_type == DCCP_PKT_RESPONSE) {
		const struct inet_connection_sock *icsk = inet_csk(sk);
		struct dccp_sock *dp = dccp_sk(sk);
		long tstamp = dccp_timestamp();

		if (!between48(DCCP_SKB_CB(skb)->dccpd_ack_seq,
			       dp->dccps_awl, dp->dccps_awh)) {
			dccp_pr_debug("invalid ackno: S.AWL=%llu, "
				      "P.ackno=%llu, S.AWH=%llu\n",
				      (unsigned long long)dp->dccps_awl,
			   (unsigned long long)DCCP_SKB_CB(skb)->dccpd_ack_seq,
				      (unsigned long long)dp->dccps_awh);
			goto out_invalid_packet;
		}

		/*
		 * If option processing (Step 8) failed, return 1 here so that
		 * dccp_v4_do_rcv() sends a Reset. The Reset code depends on
		 * the option type and is set in dccp_parse_options().
		 */
		if (dccp_parse_options(sk, NULL, skb))
			return 1;

		/* Obtain usec RTT sample from SYN exchange (used by TFRC). */
		if (likely(dp->dccps_options_received.dccpor_timestamp_echo))
			dp->dccps_syn_rtt = dccp_sample_rtt(sk, 10 * (tstamp -
			    dp->dccps_options_received.dccpor_timestamp_echo));

		/* Stop the REQUEST timer */
		inet_csk_clear_xmit_timer(sk, ICSK_TIME_RETRANS);
		WARN_ON(sk->sk_send_head == NULL);
		kfree_skb(sk->sk_send_head);
		sk->sk_send_head = NULL;

		/*
		 * Set ISR, GSR from packet. ISS was set in dccp_v{4,6}_connect
		 * and GSS in dccp_transmit_skb(). Setting AWL/AWH and SWL/SWH
		 * is done as part of activating the feature values below, since
		 * these settings depend on the local/remote Sequence Window
		 * features, which were undefined or not confirmed until now.
		 */
		dp->dccps_gsr = dp->dccps_isr = DCCP_SKB_CB(skb)->dccpd_seq;

		dccp_sync_mss(sk, icsk->icsk_pmtu_cookie);

		/*
		 *    Step 10: Process REQUEST state (second part)
		 *       If S.state == REQUEST,
		 *	  / * If we get here, P is a valid Response from the
		 *	      server (see Step 4), and we should move to
		 *	      PARTOPEN state. PARTOPEN means send an Ack,
		 *	      don't send Data packets, retransmit Acks
		 *	      periodically, and always include any Init Cookie
		 *	      from the Response * /
		 *	  S.state := PARTOPEN
		 *	  Set PARTOPEN timer
		 *	  Continue with S.state == PARTOPEN
		 *	  / * Step 12 will send the Ack completing the
		 *	      three-way handshake * /
		 */
		dccp_set_state(sk, DCCP_PARTOPEN);

		/*
		 * If feature negotiation was successful, activate features now;
		 * an activation failure means that this host could not activate
		 * one ore more features (e.g. insufficient memory), which would
		 * leave at least one feature in an undefined state.
		 */
		if (dccp_feat_activate_values(sk, &dp->dccps_featneg))
			goto unable_to_proceed;

		/* Make sure socket is routed, for correct metrics. */
		icsk->icsk_af_ops->rebuild_header(sk);

		if (!sock_flag(sk, SOCK_DEAD)) {
			sk->sk_state_change(sk);
			sk_wake_async(sk, SOCK_WAKE_IO, POLL_OUT);
		}

		if (sk->sk_write_pending || icsk->icsk_ack.pingpong ||
		    icsk->icsk_accept_queue.rskq_defer_accept) {
			/* Save one ACK. Data will be ready after
			 * several ticks, if write_pending is set.
			 *
			 * It may be deleted, but with this feature tcpdumps
			 * look so _wonderfully_ clever, that I was not able
			 * to stand against the temptation 8)     --ANK
			 */
			/*
			 * OK, in DCCP we can as well do a similar trick, its
			 * even in the draft, but there is no need for us to
			 * schedule an ack here, as dccp_sendmsg does this for
			 * us, also stated in the draft. -acme
			 */
			__kfree_skb(skb);
			return 0;
		}
		dccp_send_ack(sk);
		return -1;
	}

out_invalid_packet:
	/* dccp_v4_do_rcv will send a reset */
	DCCP_SKB_CB(skb)->dccpd_reset_code = DCCP_RESET_CODE_PACKET_ERROR;
	return 1;

unable_to_proceed:
	DCCP_SKB_CB(skb)->dccpd_reset_code = DCCP_RESET_CODE_ABORTED;
	/*
	 * We mark this socket as no longer usable, so that the loop in
	 * dccp_sendmsg() terminates and the application gets notified.
	 */
	dccp_set_state(sk, DCCP_CLOSED);
	sk->sk_err = ECOMM;
	return 1;
}
