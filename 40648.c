static void sctp_send_stale_cookie_err(struct net *net,
				       const struct sctp_endpoint *ep,
				       const struct sctp_association *asoc,
				       const struct sctp_chunk *chunk,
				       sctp_cmd_seq_t *commands,
				       struct sctp_chunk *err_chunk)
{
	struct sctp_packet *packet;

	if (err_chunk) {
		packet = sctp_ootb_pkt_new(net, asoc, chunk);
		if (packet) {
			struct sctp_signed_cookie *cookie;

			/* Override the OOTB vtag from the cookie. */
			cookie = chunk->subh.cookie_hdr;
			packet->vtag = cookie->c.peer_vtag;

			/* Set the skb to the belonging sock for accounting. */
			err_chunk->skb->sk = ep->base.sk;
			sctp_packet_append_chunk(packet, err_chunk);
			sctp_add_cmd_sf(commands, SCTP_CMD_SEND_PKT,
					SCTP_PACKET(packet));
			SCTP_INC_STATS(net, SCTP_MIB_OUTCTRLCHUNKS);
		} else
			sctp_chunk_free (err_chunk);
	}
}
