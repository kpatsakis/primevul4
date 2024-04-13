static int ccid3_hc_tx_parse_options(struct sock *sk, u8 packet_type,
				     u8 option, u8 *optval, u8 optlen)
{
	struct ccid3_hc_tx_sock *hc = ccid3_hc_tx_sk(sk);
	__be32 opt_val;

	switch (option) {
	case TFRC_OPT_RECEIVE_RATE:
	case TFRC_OPT_LOSS_EVENT_RATE:
		/* Must be ignored on Data packets, cf. RFC 4342 8.3 and 8.5 */
		if (packet_type == DCCP_PKT_DATA)
			break;
		if (unlikely(optlen != 4)) {
			DCCP_WARN("%s(%p), invalid len %d for %u\n",
				  dccp_role(sk), sk, optlen, option);
			return -EINVAL;
		}
		opt_val = ntohl(get_unaligned((__be32 *)optval));

		if (option == TFRC_OPT_RECEIVE_RATE) {
			/* Receive Rate is kept in units of 64 bytes/second */
			hc->tx_x_recv = opt_val;
			hc->tx_x_recv <<= 6;

			ccid3_pr_debug("%s(%p), RECEIVE_RATE=%u\n",
				       dccp_role(sk), sk, opt_val);
		} else {
			/* Update the fixpoint Loss Event Rate fraction */
			hc->tx_p = tfrc_invert_loss_event_rate(opt_val);

			ccid3_pr_debug("%s(%p), LOSS_EVENT_RATE=%u\n",
				       dccp_role(sk), sk, opt_val);
		}
	}
	return 0;
}
