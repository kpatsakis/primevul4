static struct sctp_packet *sctp_abort_pkt_new(struct net *net,
				  const struct sctp_endpoint *ep,
				  const struct sctp_association *asoc,
				  struct sctp_chunk *chunk,
				  const void *payload,
				  size_t paylen)
{
	struct sctp_packet *packet;
	struct sctp_chunk *abort;

	packet = sctp_ootb_pkt_new(net, asoc, chunk);

	if (packet) {
		/* Make an ABORT.
		 * The T bit will be set if the asoc is NULL.
		 */
		abort = sctp_make_abort(asoc, chunk, paylen);
		if (!abort) {
			sctp_ootb_pkt_free(packet);
			return NULL;
		}

		/* Reflect vtag if T-Bit is set */
		if (sctp_test_T_bit(abort))
			packet->vtag = ntohl(chunk->sctp_hdr->vtag);

		/* Add specified error causes, i.e., payload, to the
		 * end of the chunk.
		 */
		sctp_addto_chunk(abort, paylen, payload);

		/* Set the skb to the belonging sock for accounting.  */
		abort->skb->sk = ep->base.sk;

		sctp_packet_append_chunk(packet, abort);

	}

	return packet;
}
