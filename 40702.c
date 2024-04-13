sctp_disposition_t sctp_sf_ignore_other(struct net *net,
					const struct sctp_endpoint *ep,
					const struct sctp_association *asoc,
					const sctp_subtype_t type,
					void *arg,
					sctp_cmd_seq_t *commands)
{
	SCTP_DEBUG_PRINTK("The event other type %d is ignored\n", type.other);
	return SCTP_DISPOSITION_DISCARD;
}
