static void ccid3_hc_rx_packet_recv(struct sock *sk, struct sk_buff *skb)
{
	struct ccid3_hc_rx_sock *hc = ccid3_hc_rx_sk(sk);
	enum ccid3_fback_type do_feedback = CCID3_FBACK_NONE;
	const u64 ndp = dccp_sk(sk)->dccps_options_received.dccpor_ndp;
	const bool is_data_packet = dccp_data_packet(skb);

	if (unlikely(hc->rx_state == TFRC_RSTATE_NO_DATA)) {
		if (is_data_packet) {
			const u32 payload = skb->len - dccp_hdr(skb)->dccph_doff * 4;
			do_feedback = CCID3_FBACK_INITIAL;
			ccid3_hc_rx_set_state(sk, TFRC_RSTATE_DATA);
			hc->rx_s = payload;
			/*
			 * Not necessary to update rx_bytes_recv here,
			 * since X_recv = 0 for the first feedback packet (cf.
			 * RFC 3448, 6.3) -- gerrit
			 */
		}
		goto update_records;
	}

	if (tfrc_rx_hist_duplicate(&hc->rx_hist, skb))
		return; /* done receiving */

	if (is_data_packet) {
		const u32 payload = skb->len - dccp_hdr(skb)->dccph_doff * 4;
		/*
		 * Update moving-average of s and the sum of received payload bytes
		 */
		hc->rx_s = tfrc_ewma(hc->rx_s, payload, 9);
		hc->rx_bytes_recv += payload;
	}

	/*
	 * Perform loss detection and handle pending losses
	 */
	if (tfrc_rx_handle_loss(&hc->rx_hist, &hc->rx_li_hist,
				skb, ndp, ccid3_first_li, sk)) {
		do_feedback = CCID3_FBACK_PARAM_CHANGE;
		goto done_receiving;
	}

	if (tfrc_rx_hist_loss_pending(&hc->rx_hist))
		return; /* done receiving */

	/*
	 * Handle data packets: RTT sampling and monitoring p
	 */
	if (unlikely(!is_data_packet))
		goto update_records;

	if (!tfrc_lh_is_initialised(&hc->rx_li_hist)) {
		const u32 sample = tfrc_rx_hist_sample_rtt(&hc->rx_hist, skb);
		/*
		 * Empty loss history: no loss so far, hence p stays 0.
		 * Sample RTT values, since an RTT estimate is required for the
		 * computation of p when the first loss occurs; RFC 3448, 6.3.1.
		 */
		if (sample != 0)
			hc->rx_rtt = tfrc_ewma(hc->rx_rtt, sample, 9);

	} else if (tfrc_lh_update_i_mean(&hc->rx_li_hist, skb)) {
		/*
		 * Step (3) of [RFC 3448, 6.1]: Recompute I_mean and, if I_mean
		 * has decreased (resp. p has increased), send feedback now.
		 */
		do_feedback = CCID3_FBACK_PARAM_CHANGE;
	}

	/*
	 * Check if the periodic once-per-RTT feedback is due; RFC 4342, 10.3
	 */
	if (SUB16(dccp_hdr(skb)->dccph_ccval, hc->rx_last_counter) > 3)
		do_feedback = CCID3_FBACK_PERIODIC;

update_records:
	tfrc_rx_hist_add_packet(&hc->rx_hist, skb, ndp);

done_receiving:
	if (do_feedback)
		ccid3_hc_rx_send_feedback(sk, skb, do_feedback);
}
