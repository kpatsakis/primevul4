static sctp_disposition_t sctp_sf_tabort_8_4_8(struct net *net,
					const struct sctp_endpoint *ep,
					const struct sctp_association *asoc,
					const sctp_subtype_t type,
					void *arg,
					sctp_cmd_seq_t *commands)
{
	struct sctp_packet *packet = NULL;
	struct sctp_chunk *chunk = arg;
	struct sctp_chunk *abort;

	packet = sctp_ootb_pkt_new(net, asoc, chunk);

	if (packet) {
		/* Make an ABORT. The T bit will be set if the asoc
		 * is NULL.
		 */
		abort = sctp_make_abort(asoc, chunk, 0);
		if (!abort) {
			sctp_ootb_pkt_free(packet);
			return SCTP_DISPOSITION_NOMEM;
		}

		/* Reflect vtag if T-Bit is set */
		if (sctp_test_T_bit(abort))
			packet->vtag = ntohl(chunk->sctp_hdr->vtag);

		/* Set the skb to the belonging sock for accounting.  */
		abort->skb->sk = ep->base.sk;

		sctp_packet_append_chunk(packet, abort);

		sctp_add_cmd_sf(commands, SCTP_CMD_SEND_PKT,
				SCTP_PACKET(packet));

		SCTP_INC_STATS(net, SCTP_MIB_OUTCTRLCHUNKS);

		sctp_sf_pdiscard(net, ep, asoc, type, arg, commands);
		return SCTP_DISPOSITION_CONSUME;
	}

	return SCTP_DISPOSITION_NOMEM;
}
