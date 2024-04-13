static int sctp_inet_af_supported(sa_family_t family, struct sctp_sock *sp)
{
	/* PF_INET only supports AF_INET addresses. */
	return AF_INET == family;
}
