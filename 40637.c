static int set_sadb_kmaddress(struct sk_buff *skb, const struct xfrm_kmaddress *k)
{
	struct sadb_x_kmaddress *kma;
	u8 *sa;
	int family = k->family;
	int socklen = pfkey_sockaddr_len(family);
	int size_req;

	size_req = (sizeof(struct sadb_x_kmaddress) +
		    pfkey_sockaddr_pair_size(family));

	kma = (struct sadb_x_kmaddress *)skb_put(skb, size_req);
	memset(kma, 0, size_req);
	kma->sadb_x_kmaddress_len = size_req / 8;
	kma->sadb_x_kmaddress_exttype = SADB_X_EXT_KMADDRESS;
	kma->sadb_x_kmaddress_reserved = k->reserved;

	sa = (u8 *)(kma + 1);
	if (!pfkey_sockaddr_fill(&k->local, 0, (struct sockaddr *)sa, family) ||
	    !pfkey_sockaddr_fill(&k->remote, 0, (struct sockaddr *)(sa+socklen), family))
		return -EINVAL;

	return 0;
}
