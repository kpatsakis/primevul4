static sctp_disposition_t sctp_sf_shut_8_4_5(struct net *net,
					     const struct sctp_endpoint *ep,
					     const struct sctp_association *asoc,
					     const sctp_subtype_t type,
					     void *arg,
					     sctp_cmd_seq_t *commands)
{
	struct sctp_packet *packet = NULL;
	struct sctp_chunk *chunk = arg;
	struct sctp_chunk *shut;

	packet = sctp_ootb_pkt_new(net, asoc, chunk);

	if (packet) {
		/* Make an SHUTDOWN_COMPLETE.
		 * The T bit will be set if the asoc is NULL.
		 */
		shut = sctp_make_shutdown_complete(asoc, chunk);
		if (!shut) {
			sctp_ootb_pkt_free(packet);
			return SCTP_DISPOSITION_NOMEM;
		}

		/* Reflect vtag if T-Bit is set */
		if (sctp_test_T_bit(shut))
			packet->vtag = ntohl(chunk->sctp_hdr->vtag);

		/* Set the skb to the belonging sock for accounting.  */
		shut->skb->sk = ep->base.sk;

		sctp_packet_append_chunk(packet, shut);

		sctp_add_cmd_sf(commands, SCTP_CMD_SEND_PKT,
				SCTP_PACKET(packet));

		SCTP_INC_STATS(net, SCTP_MIB_OUTCTRLCHUNKS);

		/* If the chunk length is invalid, we don't want to process
		 * the reset of the packet.
		 */
		if (!sctp_chunk_length_valid(chunk, sizeof(sctp_chunkhdr_t)))
			return sctp_sf_pdiscard(net, ep, asoc, type, arg, commands);

		/* We need to discard the rest of the packet to prevent
		 * potential bomming attacks from additional bundled chunks.
		 * This is documented in SCTP Threats ID.
		 */
		return sctp_sf_pdiscard(net, ep, asoc, type, arg, commands);
	}

	return SCTP_DISPOSITION_NOMEM;
}
