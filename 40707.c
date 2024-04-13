sctp_disposition_t sctp_sf_pdiscard(struct net *net,
				    const struct sctp_endpoint *ep,
				    const struct sctp_association *asoc,
				    const sctp_subtype_t type,
				    void *arg,
				    sctp_cmd_seq_t *commands)
{
	SCTP_INC_STATS(net, SCTP_MIB_IN_PKT_DISCARDS);
	sctp_add_cmd_sf(commands, SCTP_CMD_DISCARD_PACKET, SCTP_NULL());

	return SCTP_DISPOSITION_CONSUME;
}
