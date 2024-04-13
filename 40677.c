sctp_disposition_t sctp_sf_do_9_1_prm_abort(
	struct net *net,
	const struct sctp_endpoint *ep,
	const struct sctp_association *asoc,
	const sctp_subtype_t type,
	void *arg,
	sctp_cmd_seq_t *commands)
{
	/* From 9.1 Abort of an Association
	 * Upon receipt of the ABORT primitive from its upper
	 * layer, the endpoint enters CLOSED state and
	 * discard all outstanding data has been
	 * acknowledged by its peer. The endpoint accepts no new data
	 * from its upper layer, but retransmits data to the far end
	 * if necessary to fill gaps.
	 */
	struct sctp_chunk *abort = arg;
	sctp_disposition_t retval;

	retval = SCTP_DISPOSITION_CONSUME;

	sctp_add_cmd_sf(commands, SCTP_CMD_REPLY, SCTP_CHUNK(abort));

	/* Even if we can't send the ABORT due to low memory delete the
	 * TCB.  This is a departure from our typical NOMEM handling.
	 */

	sctp_add_cmd_sf(commands, SCTP_CMD_SET_SK_ERR,
			SCTP_ERROR(ECONNABORTED));
	/* Delete the established association. */
	sctp_add_cmd_sf(commands, SCTP_CMD_ASSOC_FAILED,
			SCTP_PERR(SCTP_ERROR_USER_ABORT));

	SCTP_INC_STATS(net, SCTP_MIB_ABORTEDS);
	SCTP_DEC_STATS(net, SCTP_MIB_CURRESTAB);

	return retval;
}
