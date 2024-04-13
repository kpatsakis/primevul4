static int set_sadb_address(struct sk_buff *skb, int sasize, int type,
			    const struct xfrm_selector *sel)
{
	struct sadb_address *addr;
	addr = (struct sadb_address *)skb_put(skb, sizeof(struct sadb_address) + sasize);
	addr->sadb_address_len = (sizeof(struct sadb_address) + sasize)/8;
	addr->sadb_address_exttype = type;
	addr->sadb_address_proto = sel->proto;
	addr->sadb_address_reserved = 0;

	switch (type) {
	case SADB_EXT_ADDRESS_SRC:
		addr->sadb_address_prefixlen = sel->prefixlen_s;
		pfkey_sockaddr_fill(&sel->saddr, 0,
				    (struct sockaddr *)(addr + 1),
				    sel->family);
		break;
	case SADB_EXT_ADDRESS_DST:
		addr->sadb_address_prefixlen = sel->prefixlen_d;
		pfkey_sockaddr_fill(&sel->daddr, 0,
				    (struct sockaddr *)(addr + 1),
				    sel->family);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}
