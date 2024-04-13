sctp_disposition_t sctp_sf_do_6_2_sack(struct net *net,
				       const struct sctp_endpoint *ep,
				       const struct sctp_association *asoc,
				       const sctp_subtype_t type,
				       void *arg,
				       sctp_cmd_seq_t *commands)
{
	SCTP_INC_STATS(net, SCTP_MIB_DELAY_SACK_EXPIREDS);
	sctp_add_cmd_sf(commands, SCTP_CMD_GEN_SACK, SCTP_FORCE());
	return SCTP_DISPOSITION_CONSUME;
}
