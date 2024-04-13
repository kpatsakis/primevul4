static int pfkey_xfrm_policy2msg_size(const struct xfrm_policy *xp)
{
	const struct xfrm_tmpl *t;
	int sockaddr_size = pfkey_sockaddr_size(xp->family);
	int socklen = 0;
	int i;

	for (i=0; i<xp->xfrm_nr; i++) {
		t = xp->xfrm_vec + i;
		socklen += pfkey_sockaddr_len(t->encap_family);
	}

	return sizeof(struct sadb_msg) +
		(sizeof(struct sadb_lifetime) * 3) +
		(sizeof(struct sadb_address) * 2) +
		(sockaddr_size * 2) +
		sizeof(struct sadb_x_policy) +
		(xp->xfrm_nr * sizeof(struct sadb_x_ipsecrequest)) +
		(socklen * 2) +
		pfkey_xfrm_policy2sec_ctx_size(xp);
}
