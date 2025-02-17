sctp_disposition_t sctp_sf_eat_fwd_tsn(struct net *net,
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

	/* Make sure that the FORWARD_TSN chunk has valid length.  */
	if (!sctp_chunk_length_valid(chunk, sizeof(struct sctp_fwdtsn_chunk)))
		return sctp_sf_violation_chunklen(net, ep, asoc, type, arg,
						  commands);

	fwdtsn_hdr = (struct sctp_fwdtsn_hdr *)chunk->skb->data;
	chunk->subh.fwdtsn_hdr = fwdtsn_hdr;
	len = ntohs(chunk->chunk_hdr->length);
	len -= sizeof(struct sctp_chunkhdr);
	skb_pull(chunk->skb, len);

	tsn = ntohl(fwdtsn_hdr->new_cum_tsn);
	SCTP_DEBUG_PRINTK("%s: TSN 0x%x.\n", __func__, tsn);

	/* The TSN is too high--silently discard the chunk and count on it
	 * getting retransmitted later.
	 */
	if (sctp_tsnmap_check(&asoc->peer.tsn_map, tsn) < 0)
		goto discard_noforce;

	/* Silently discard the chunk if stream-id is not valid */
	sctp_walk_fwdtsn(skip, chunk) {
		if (ntohs(skip->stream) >= asoc->c.sinit_max_instreams)
			goto discard_noforce;
	}

	sctp_add_cmd_sf(commands, SCTP_CMD_REPORT_FWDTSN, SCTP_U32(tsn));
	if (len > sizeof(struct sctp_fwdtsn_hdr))
		sctp_add_cmd_sf(commands, SCTP_CMD_PROCESS_FWDTSN,
				SCTP_CHUNK(chunk));

	/* Count this as receiving DATA. */
	if (asoc->autoclose) {
		sctp_add_cmd_sf(commands, SCTP_CMD_TIMER_RESTART,
				SCTP_TO(SCTP_EVENT_TIMEOUT_AUTOCLOSE));
	}

	/* FIXME: For now send a SACK, but DATA processing may
	 * send another.
	 */
	sctp_add_cmd_sf(commands, SCTP_CMD_GEN_SACK, SCTP_NOFORCE());

	return SCTP_DISPOSITION_CONSUME;

discard_noforce:
	return SCTP_DISPOSITION_DISCARD;
}
