static inline int cma_zero_addr(struct sockaddr *addr)
{
	switch (addr->sa_family) {
	case AF_INET:
		return ipv4_is_zeronet(((struct sockaddr_in *)addr)->sin_addr.s_addr);
	case AF_INET6:
		return ipv6_addr_any(&((struct sockaddr_in6 *) addr)->sin6_addr);
	case AF_IB:
		return ib_addr_any(&((struct sockaddr_ib *) addr)->sib_addr);
	default:
		return 0;
	}
}
