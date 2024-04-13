sctp_disposition_t sctp_sf_not_impl(struct net *net,
				    const struct sctp_endpoint *ep,
				    const struct sctp_association *asoc,
				    const sctp_subtype_t type,
				    void *arg,
				    sctp_cmd_seq_t *commands)
{
	return SCTP_DISPOSITION_NOT_IMPL;
}
