void sctp_hash_endpoint(struct sctp_endpoint *ep)
{
	sctp_local_bh_disable();
	__sctp_hash_endpoint(ep);
	sctp_local_bh_enable();
}
