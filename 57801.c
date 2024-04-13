static int sctp_v6_addr_to_user(struct sctp_sock *sp, union sctp_addr *addr)
{
	if (sp->v4mapped) {
		if (addr->sa.sa_family == AF_INET)
			sctp_v4_map_v6(addr);
	} else {
		if (addr->sa.sa_family == AF_INET6 &&
		    ipv6_addr_v4mapped(&addr->v6.sin6_addr))
			sctp_v6_map_v4(addr);
	}

	if (addr->sa.sa_family == AF_INET)
		return sizeof(struct sockaddr_in);
	return sizeof(struct sockaddr_in6);
}
