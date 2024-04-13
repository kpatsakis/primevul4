static struct xfrm_state *xfrm_user_state_lookup(struct net *net,
						 struct xfrm_usersa_id *p,
						 struct nlattr **attrs,
						 int *errp)
{
	struct xfrm_state *x = NULL;
	struct xfrm_mark m;
	int err;
	u32 mark = xfrm_mark_get(attrs, &m);

	if (xfrm_id_proto_match(p->proto, IPSEC_PROTO_ANY)) {
		err = -ESRCH;
		x = xfrm_state_lookup(net, mark, &p->daddr, p->spi, p->proto, p->family);
	} else {
		xfrm_address_t *saddr = NULL;

		verify_one_addr(attrs, XFRMA_SRCADDR, &saddr);
		if (!saddr) {
			err = -EINVAL;
			goto out;
		}

		err = -ESRCH;
		x = xfrm_state_lookup_byaddr(net, mark,
					     &p->daddr, saddr,
					     p->proto, p->family);
	}

 out:
	if (!x && errp)
		*errp = err;
	return x;
}
