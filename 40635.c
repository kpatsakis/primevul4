static int present_and_same_family(const struct sadb_address *src,
				   const struct sadb_address *dst)
{
	const struct sockaddr *s_addr, *d_addr;

	if (!src || !dst)
		return 0;

	s_addr = (const struct sockaddr *)(src + 1);
	d_addr = (const struct sockaddr *)(dst + 1);
	if (s_addr->sa_family != d_addr->sa_family)
		return 0;
	if (s_addr->sa_family != AF_INET
#if IS_ENABLED(CONFIG_IPV6)
	    && s_addr->sa_family != AF_INET6
#endif
		)
		return 0;

	return 1;
}
