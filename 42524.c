void sctp_hash_established(struct sctp_association *asoc)
{
	if (asoc->temp)
		return;

	sctp_local_bh_disable();
	__sctp_hash_established(asoc);
	sctp_local_bh_enable();
}
