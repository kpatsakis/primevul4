static sctp_disposition_t sctp_sf_violation_paramlen(
				     struct net *net,
				     const struct sctp_endpoint *ep,
				     const struct sctp_association *asoc,
				     const sctp_subtype_t type,
				     void *arg, void *ext,
				     sctp_cmd_seq_t *commands)
{
	struct sctp_chunk *chunk =  arg;
	struct sctp_paramhdr *param = ext;
	struct sctp_chunk *abort = NULL;

	if (sctp_auth_recv_cid(SCTP_CID_ABORT, asoc))
		goto discard;

	/* Make the abort chunk. */
	abort = sctp_make_violation_paramlen(asoc, chunk, param);
	if (!abort)
		goto nomem;

	sctp_add_cmd_sf(commands, SCTP_CMD_REPLY, SCTP_CHUNK(abort));
	SCTP_INC_STATS(net, SCTP_MIB_OUTCTRLCHUNKS);

	sctp_add_cmd_sf(commands, SCTP_CMD_SET_SK_ERR,
			SCTP_ERROR(ECONNABORTED));
	sctp_add_cmd_sf(commands, SCTP_CMD_ASSOC_FAILED,
			SCTP_PERR(SCTP_ERROR_PROTO_VIOLATION));
	SCTP_DEC_STATS(net, SCTP_MIB_CURRESTAB);
	SCTP_INC_STATS(net, SCTP_MIB_ABORTEDS);

discard:
	sctp_sf_pdiscard(net, ep, asoc, SCTP_ST_CHUNK(0), arg, commands);
	return SCTP_DISPOSITION_ABORT;
nomem:
	return SCTP_DISPOSITION_NOMEM;
}
