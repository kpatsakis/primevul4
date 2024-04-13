sctp_disposition_t sctp_sf_do_no_pending_tsn(
	struct net *net,
	const struct sctp_endpoint *ep,
	const struct sctp_association *asoc,
	const sctp_subtype_t type,
	void *arg,
	sctp_cmd_seq_t *commands)
{
	struct sctp_ulpevent *event;

	event = sctp_ulpevent_make_sender_dry_event(asoc, GFP_ATOMIC);
	if (!event)
		return SCTP_DISPOSITION_NOMEM;

	sctp_add_cmd_sf(commands, SCTP_CMD_EVENT_ULP, SCTP_ULPEVENT(event));

	return SCTP_DISPOSITION_CONSUME;
}
