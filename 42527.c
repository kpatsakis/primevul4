struct sctp_association *sctp_lookup_association(const union sctp_addr *laddr,
						 const union sctp_addr *paddr,
					    struct sctp_transport **transportp)
{
	struct sctp_association *asoc;

	sctp_local_bh_disable();
	asoc = __sctp_lookup_association(laddr, paddr, transportp);
	sctp_local_bh_enable();

	return asoc;
}
