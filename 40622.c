static inline int pfkey_sockaddr_len(sa_family_t family)
{
	switch (family) {
	case AF_INET:
		return sizeof(struct sockaddr_in);
#if IS_ENABLED(CONFIG_IPV6)
	case AF_INET6:
		return sizeof(struct sockaddr_in6);
#endif
	}
	return 0;
}
