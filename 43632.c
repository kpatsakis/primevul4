sctp_disposition_t sctp_sf_eat_fwd_tsn_fast(
	struct net *net,
	const struct sctp_endpoint *ep,
	const struct sctp_association *asoc,
	const sctp_subtype_t type,
	void *arg,
	sctp_cmd_seq_t *commands)
{
	struct sctp_chunk *chunk = arg;
	struct sctp_fwdtsn_hdr *fwdtsn_hdr;
	struct sctp_fwdtsn_skip *skip;
	__u16 len;
	__u32 tsn;

	if (!sctp_vtag_verify(chunk, asoc)) {
		sctp_add_cmd_sf(commands, SCTP_CMD_REPORT_BAD_TAG,
				SCTP_NULL());
		return sctp_sf_pdiscard(net, ep, asoc, type, arg, commands);
	}

	/* Make sure that the FORWARD_TSN chunk has a valid length.  */
	if (!sctp_chunk_length_valid(chunk, sizeof(struct sctp_fwdtsn_chunk)))
		return sctp_sf_violation_chunklen(net, ep, asoc, type, arg,
						  commands);

	fwdtsn_hdr = (struct sctp_fwdtsn_hdr *)chunk->skb->data;
	chunk->subh.fwdtsn_hdr = fwdtsn_hdr;
	len = ntohs(chunk->chunk_hdr->length);
	len -= sizeof(struct sctp_chunkhdr);
	skb_pull(chunk->skb, len);

	tsn = ntohl(fwdtsn_hdr->new_cum_tsn);
	pr_debug("%s: TSN 0x%x\n", __func__, tsn);

	/* The TSN is too high--silently discard the chunk and count on it
	 * getting retransmitted later.
	 */
	if (sctp_tsnmap_check(&asoc->peer.tsn_map, tsn) < 0)
		goto gen_shutdown;

	/* Silently discard the chunk if stream-id is not valid */
	sctp_walk_fwdtsn(skip, chunk) {
		if (ntohs(skip->stream) >= asoc->c.sinit_max_instreams)
			goto gen_shutdown;
	}

	sctp_add_cmd_sf(commands, SCTP_CMD_REPORT_FWDTSN, SCTP_U32(tsn));
	if (len > sizeof(struct sctp_fwdtsn_hdr))
		sctp_add_cmd_sf(commands, SCTP_CMD_PROCESS_FWDTSN,
				SCTP_CHUNK(chunk));

	/* Go a head and force a SACK, since we are shutting down. */
gen_shutdown:
	/* Implementor's Guide.
	 *
	 * While in SHUTDOWN-SENT state, the SHUTDOWN sender MUST immediately
	 * respond to each received packet containing one or more DATA chunk(s)
	 * with a SACK, a SHUTDOWN chunk, and restart the T2-shutdown timer
	 */
	sctp_add_cmd_sf(commands, SCTP_CMD_GEN_SHUTDOWN, SCTP_NULL());
	sctp_add_cmd_sf(commands, SCTP_CMD_GEN_SACK, SCTP_FORCE());
	sctp_add_cmd_sf(commands, SCTP_CMD_TIMER_RESTART,
			SCTP_TO(SCTP_EVENT_TIMEOUT_T2_SHUTDOWN));

	return SCTP_DISPOSITION_CONSUME;
}
