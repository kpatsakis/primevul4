void ndisc_send_ns(struct net_device *dev, struct neighbour *neigh,
		   const struct in6_addr *solicit,
		   const struct in6_addr *daddr, const struct in6_addr *saddr)
{
	struct sk_buff *skb;
	struct in6_addr addr_buf;
	int inc_opt = dev->addr_len;
	int optlen = 0;
	struct nd_msg *msg;

	if (saddr == NULL) {
		if (ipv6_get_lladdr(dev, &addr_buf,
				   (IFA_F_TENTATIVE|IFA_F_OPTIMISTIC)))
			return;
		saddr = &addr_buf;
	}

	if (ipv6_addr_any(saddr))
		inc_opt = false;
	if (inc_opt)
		optlen += ndisc_opt_addr_space(dev);

	skb = ndisc_alloc_skb(dev, sizeof(*msg) + optlen);
	if (!skb)
		return;

	msg = (struct nd_msg *)skb_put(skb, sizeof(*msg));
	*msg = (struct nd_msg) {
		.icmph = {
			.icmp6_type = NDISC_NEIGHBOUR_SOLICITATION,
		},
		.target = *solicit,
	};

	if (inc_opt)
		ndisc_fill_addr_option(skb, ND_OPT_SOURCE_LL_ADDR,
				       dev->dev_addr);

	ndisc_send_skb(skb, daddr, saddr);
}
