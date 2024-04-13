static int sctp_inet_cmp_addr(const union sctp_addr *addr1,
			      const union sctp_addr *addr2,
			      struct sctp_sock *opt)
{
	/* PF_INET only supports AF_INET addresses. */
	if (addr1->sa.sa_family != addr2->sa.sa_family)
		return 0;
	if (htonl(INADDR_ANY) == addr1->v4.sin_addr.s_addr ||
	    htonl(INADDR_ANY) == addr2->v4.sin_addr.s_addr)
		return 1;
	if (addr1->v4.sin_addr.s_addr == addr2->v4.sin_addr.s_addr)
		return 1;

	return 0;
}
