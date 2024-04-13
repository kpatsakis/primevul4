sctp_disposition_t sctp_sf_timer_ignore(struct net *net,
					const struct sctp_endpoint *ep,
					const struct sctp_association *asoc,
					const sctp_subtype_t type,
					void *arg,
					sctp_cmd_seq_t *commands)
{
	pr_debug("%s: timer %d ignored\n", __func__, type.chunk);

	return SCTP_DISPOSITION_CONSUME;
}
