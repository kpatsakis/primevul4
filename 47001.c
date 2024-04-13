static int sctp_v4_is_any(const union sctp_addr *addr)
{
	return htonl(INADDR_ANY) == addr->v4.sin_addr.s_addr;
}
