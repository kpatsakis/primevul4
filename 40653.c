sctp_disposition_t sctp_sf_beat_8_3(struct net *net,
				    const struct sctp_endpoint *ep,
				    const struct sctp_association *asoc,
				    const sctp_subtype_t type,
				    void *arg,
				    sctp_cmd_seq_t *commands)
{
	sctp_paramhdr_t *param_hdr;
	struct sctp_chunk *chunk = arg;
	struct sctp_chunk *reply;
	size_t paylen = 0;

	if (!sctp_vtag_verify(chunk, asoc))
		return sctp_sf_pdiscard(net, ep, asoc, type, arg, commands);

	/* Make sure that the HEARTBEAT chunk has a valid length. */
	if (!sctp_chunk_length_valid(chunk, sizeof(sctp_heartbeat_chunk_t)))
		return sctp_sf_violation_chunklen(net, ep, asoc, type, arg,
						  commands);

	/* 8.3 The receiver of the HEARTBEAT should immediately
	 * respond with a HEARTBEAT ACK that contains the Heartbeat
	 * Information field copied from the received HEARTBEAT chunk.
	 */
	chunk->subh.hb_hdr = (sctp_heartbeathdr_t *) chunk->skb->data;
	param_hdr = (sctp_paramhdr_t *) chunk->subh.hb_hdr;
	paylen = ntohs(chunk->chunk_hdr->length) - sizeof(sctp_chunkhdr_t);

	if (ntohs(param_hdr->length) > paylen)
		return sctp_sf_violation_paramlen(net, ep, asoc, type, arg,
						  param_hdr, commands);

	if (!pskb_pull(chunk->skb, paylen))
		goto nomem;

	reply = sctp_make_heartbeat_ack(asoc, chunk, param_hdr, paylen);
	if (!reply)
		goto nomem;

	sctp_add_cmd_sf(commands, SCTP_CMD_REPLY, SCTP_CHUNK(reply));
	return SCTP_DISPOSITION_CONSUME;

nomem:
	return SCTP_DISPOSITION_NOMEM;
}
