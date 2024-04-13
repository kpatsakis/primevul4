static sctp_disposition_t __sctp_sf_do_9_1_abort(struct net *net,
					const struct sctp_endpoint *ep,
					const struct sctp_association *asoc,
					const sctp_subtype_t type,
					void *arg,
					sctp_cmd_seq_t *commands)
{
	struct sctp_chunk *chunk = arg;
	unsigned int len;
	__be16 error = SCTP_ERROR_NO_ERROR;

	/* See if we have an error cause code in the chunk.  */
	len = ntohs(chunk->chunk_hdr->length);
	if (len >= sizeof(struct sctp_chunkhdr) + sizeof(struct sctp_errhdr)) {

		sctp_errhdr_t *err;
		sctp_walk_errors(err, chunk->chunk_hdr);
		if ((void *)err != (void *)chunk->chunk_end)
			return sctp_sf_pdiscard(net, ep, asoc, type, arg, commands);

		error = ((sctp_errhdr_t *)chunk->skb->data)->cause;
	}

	sctp_add_cmd_sf(commands, SCTP_CMD_SET_SK_ERR, SCTP_ERROR(ECONNRESET));
	/* ASSOC_FAILED will DELETE_TCB. */
	sctp_add_cmd_sf(commands, SCTP_CMD_ASSOC_FAILED, SCTP_PERR(error));
	SCTP_INC_STATS(net, SCTP_MIB_ABORTEDS);
	SCTP_DEC_STATS(net, SCTP_MIB_CURRESTAB);

	return SCTP_DISPOSITION_ABORT;
}
