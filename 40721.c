sctp_disposition_t sctp_sf_t5_timer_expire(struct net *net,
					   const struct sctp_endpoint *ep,
					   const struct sctp_association *asoc,
					   const sctp_subtype_t type,
					   void *arg,
					   sctp_cmd_seq_t *commands)
{
	struct sctp_chunk *reply = NULL;

	SCTP_DEBUG_PRINTK("Timer T5 expired.\n");
	SCTP_INC_STATS(net, SCTP_MIB_T5_SHUTDOWN_GUARD_EXPIREDS);

	reply = sctp_make_abort(asoc, NULL, 0);
	if (!reply)
		goto nomem;

	sctp_add_cmd_sf(commands, SCTP_CMD_REPLY, SCTP_CHUNK(reply));
	sctp_add_cmd_sf(commands, SCTP_CMD_SET_SK_ERR,
			SCTP_ERROR(ETIMEDOUT));
	sctp_add_cmd_sf(commands, SCTP_CMD_ASSOC_FAILED,
			SCTP_PERR(SCTP_ERROR_NO_ERROR));

	SCTP_INC_STATS(net, SCTP_MIB_ABORTEDS);
	SCTP_DEC_STATS(net, SCTP_MIB_CURRESTAB);

	return SCTP_DISPOSITION_DELETE_TCB;
nomem:
	return SCTP_DISPOSITION_NOMEM;
}
