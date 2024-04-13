static int cma_addr_cmp(struct sockaddr *src, struct sockaddr *dst)
{
	if (src->sa_family != dst->sa_family)
		return -1;

	switch (src->sa_family) {
	case AF_INET:
		return ((struct sockaddr_in *) src)->sin_addr.s_addr !=
		       ((struct sockaddr_in *) dst)->sin_addr.s_addr;
	case AF_INET6:
		return ipv6_addr_cmp(&((struct sockaddr_in6 *) src)->sin6_addr,
				     &((struct sockaddr_in6 *) dst)->sin6_addr);
	default:
		return ib_addr_cmp(&((struct sockaddr_ib *) src)->sib_addr,
				   &((struct sockaddr_ib *) dst)->sib_addr);
	}
}
