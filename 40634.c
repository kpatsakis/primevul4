static struct  xfrm_state *pfkey_xfrm_state_lookup(struct net *net, const struct sadb_msg *hdr, void * const *ext_hdrs)
{
	const struct sadb_sa *sa;
	const struct sadb_address *addr;
	uint16_t proto;
	unsigned short family;
	xfrm_address_t *xaddr;

	sa = ext_hdrs[SADB_EXT_SA - 1];
	if (sa == NULL)
		return NULL;

	proto = pfkey_satype2proto(hdr->sadb_msg_satype);
	if (proto == 0)
		return NULL;

	/* sadb_address_len should be checked by caller */
	addr = ext_hdrs[SADB_EXT_ADDRESS_DST - 1];
	if (addr == NULL)
		return NULL;

	family = ((const struct sockaddr *)(addr + 1))->sa_family;
	switch (family) {
	case AF_INET:
		xaddr = (xfrm_address_t *)&((const struct sockaddr_in *)(addr + 1))->sin_addr;
		break;
#if IS_ENABLED(CONFIG_IPV6)
	case AF_INET6:
		xaddr = (xfrm_address_t *)&((const struct sockaddr_in6 *)(addr + 1))->sin6_addr;
		break;
#endif
	default:
		xaddr = NULL;
	}

	if (!xaddr)
		return NULL;

	return xfrm_state_lookup(net, DUMMY_MARK, xaddr, sa->sadb_sa_spi, proto, family);
}
