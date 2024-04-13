sctp_disposition_t sctp_sf_shutdown_pending_prm_abort(
	struct net *net,
	const struct sctp_endpoint *ep,
	const struct sctp_association *asoc,
	const sctp_subtype_t type,
	void *arg,
	sctp_cmd_seq_t *commands)
{
	/* Stop the T5-shutdown guard timer.  */
	sctp_add_cmd_sf(commands, SCTP_CMD_TIMER_STOP,
			SCTP_TO(SCTP_EVENT_TIMEOUT_T5_SHUTDOWN_GUARD));

	return sctp_sf_do_9_1_prm_abort(net, ep, asoc, type, arg, commands);
}
