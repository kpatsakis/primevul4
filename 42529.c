static int sctp_rcv_ootb(struct sk_buff *skb)
{
	sctp_chunkhdr_t *ch;
	__u8 *ch_end;
	sctp_errhdr_t *err;

	ch = (sctp_chunkhdr_t *) skb->data;

	/* Scan through all the chunks in the packet.  */
	do {
		/* Break out if chunk length is less then minimal. */
		if (ntohs(ch->length) < sizeof(sctp_chunkhdr_t))
			break;

		ch_end = ((__u8 *)ch) + WORD_ROUND(ntohs(ch->length));
		if (ch_end > skb_tail_pointer(skb))
			break;

		/* RFC 8.4, 2) If the OOTB packet contains an ABORT chunk, the
		 * receiver MUST silently discard the OOTB packet and take no
		 * further action.
		 */
		if (SCTP_CID_ABORT == ch->type)
			goto discard;

		/* RFC 8.4, 6) If the packet contains a SHUTDOWN COMPLETE
		 * chunk, the receiver should silently discard the packet
		 * and take no further action.
		 */
		if (SCTP_CID_SHUTDOWN_COMPLETE == ch->type)
			goto discard;

		/* RFC 4460, 2.11.2
		 * This will discard packets with INIT chunk bundled as
		 * subsequent chunks in the packet.  When INIT is first,
		 * the normal INIT processing will discard the chunk.
		 */
		if (SCTP_CID_INIT == ch->type && (void *)ch != skb->data)
			goto discard;

		/* RFC 8.4, 7) If the packet contains a "Stale cookie" ERROR
		 * or a COOKIE ACK the SCTP Packet should be silently
		 * discarded.
		 */
		if (SCTP_CID_COOKIE_ACK == ch->type)
			goto discard;

		if (SCTP_CID_ERROR == ch->type) {
			sctp_walk_errors(err, ch) {
				if (SCTP_ERROR_STALE_COOKIE == err->cause)
					goto discard;
			}
		}

		ch = (sctp_chunkhdr_t *) ch_end;
	} while (ch_end < skb_tail_pointer(skb));

	return 0;

discard:
	return 1;
}
