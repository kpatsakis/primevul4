sctp_disposition_t sctp_sf_do_prm_send(struct net *net,
				       const struct sctp_endpoint *ep,
				       const struct sctp_association *asoc,
				       const sctp_subtype_t type,
				       void *arg,
				       sctp_cmd_seq_t *commands)
{
	struct sctp_datamsg *msg = arg;

	sctp_add_cmd_sf(commands, SCTP_CMD_SEND_MSG, SCTP_DATAMSG(msg));
	return SCTP_DISPOSITION_CONSUME;
}
