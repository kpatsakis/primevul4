static int build_mapping(struct sk_buff *skb, struct xfrm_state *x,
			 xfrm_address_t *new_saddr, __be16 new_sport)
{
	struct xfrm_user_mapping *um;
	struct nlmsghdr *nlh;

	nlh = nlmsg_put(skb, 0, 0, XFRM_MSG_MAPPING, sizeof(*um), 0);
	if (nlh == NULL)
		return -EMSGSIZE;

	um = nlmsg_data(nlh);

	memcpy(&um->id.daddr, &x->id.daddr, sizeof(um->id.daddr));
	um->id.spi = x->id.spi;
	um->id.family = x->props.family;
	um->id.proto = x->id.proto;
	memcpy(&um->new_saddr, new_saddr, sizeof(um->new_saddr));
	memcpy(&um->old_saddr, &x->props.saddr, sizeof(um->old_saddr));
	um->new_sport = new_sport;
	um->old_sport = x->encap->encap_sport;
	um->reqid = x->props.reqid;

	return nlmsg_end(skb, nlh);
}
