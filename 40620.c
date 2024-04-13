int pfkey_sockaddr_extract(const struct sockaddr *sa, xfrm_address_t *xaddr)
{
	switch (sa->sa_family) {
	case AF_INET:
		xaddr->a4 =
			((struct sockaddr_in *)sa)->sin_addr.s_addr;
		return AF_INET;
#if IS_ENABLED(CONFIG_IPV6)
	case AF_INET6:
		memcpy(xaddr->a6,
		       &((struct sockaddr_in6 *)sa)->sin6_addr,
		       sizeof(struct in6_addr));
		return AF_INET6;
#endif
	}
	return 0;
}
