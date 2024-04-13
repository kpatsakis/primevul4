static struct sctp_packet *sctp_ootb_pkt_new(struct net *net,
					     const struct sctp_association *asoc,
					     const struct sctp_chunk *chunk)
{
	struct sctp_packet *packet;
	struct sctp_transport *transport;
	__u16 sport;
	__u16 dport;
	__u32 vtag;

	/* Get the source and destination port from the inbound packet.  */
	sport = ntohs(chunk->sctp_hdr->dest);
	dport = ntohs(chunk->sctp_hdr->source);

	/* The V-tag is going to be the same as the inbound packet if no
	 * association exists, otherwise, use the peer's vtag.
	 */
	if (asoc) {
		/* Special case the INIT-ACK as there is no peer's vtag
		 * yet.
		 */
		switch(chunk->chunk_hdr->type) {
		case SCTP_CID_INIT_ACK:
		{
			sctp_initack_chunk_t *initack;

			initack = (sctp_initack_chunk_t *)chunk->chunk_hdr;
			vtag = ntohl(initack->init_hdr.init_tag);
			break;
		}
		default:
			vtag = asoc->peer.i.init_tag;
			break;
		}
	} else {
		/* Special case the INIT and stale COOKIE_ECHO as there is no
		 * vtag yet.
		 */
		switch(chunk->chunk_hdr->type) {
		case SCTP_CID_INIT:
		{
			sctp_init_chunk_t *init;

			init = (sctp_init_chunk_t *)chunk->chunk_hdr;
			vtag = ntohl(init->init_hdr.init_tag);
			break;
		}
		default:
			vtag = ntohl(chunk->sctp_hdr->vtag);
			break;
		}
	}

	/* Make a transport for the bucket, Eliza... */
	transport = sctp_transport_new(net, sctp_source(chunk), GFP_ATOMIC);
	if (!transport)
		goto nomem;

	/* Cache a route for the transport with the chunk's destination as
	 * the source address.
	 */
	sctp_transport_route(transport, (union sctp_addr *)&chunk->dest,
			     sctp_sk(net->sctp.ctl_sock));

	packet = sctp_packet_init(&transport->packet, transport, sport, dport);
	packet = sctp_packet_config(packet, vtag, 0);

	return packet;

nomem:
	return NULL;
}
