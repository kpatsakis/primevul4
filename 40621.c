static unsigned int pfkey_sockaddr_fill(const xfrm_address_t *xaddr, __be16 port,
					struct sockaddr *sa,
					unsigned short family)
{
	switch (family) {
	case AF_INET:
	    {
		struct sockaddr_in *sin = (struct sockaddr_in *)sa;
		sin->sin_family = AF_INET;
		sin->sin_port = port;
		sin->sin_addr.s_addr = xaddr->a4;
		memset(sin->sin_zero, 0, sizeof(sin->sin_zero));
		return 32;
	    }
#if IS_ENABLED(CONFIG_IPV6)
	case AF_INET6:
	    {
		struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)sa;
		sin6->sin6_family = AF_INET6;
		sin6->sin6_port = port;
		sin6->sin6_flowinfo = 0;
		sin6->sin6_addr = *(struct in6_addr *)xaddr->a6;
		sin6->sin6_scope_id = 0;
		return 128;
	    }
#endif
	}
	return 0;
}
