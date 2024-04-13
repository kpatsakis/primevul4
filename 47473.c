void ndisc_send_na(struct net_device *dev, struct neighbour *neigh,
		   const struct in6_addr *daddr,
		   const struct in6_addr *solicited_addr,
		   bool router, bool solicited, bool override, bool inc_opt)
{
	struct sk_buff *skb;
	struct in6_addr tmpaddr;
	struct inet6_ifaddr *ifp;
	const struct in6_addr *src_addr;
	struct nd_msg *msg;
	int optlen = 0;

	/* for anycast or proxy, solicited_addr != src_addr */
	ifp = ipv6_get_ifaddr(dev_net(dev), solicited_addr, dev, 1);
	if (ifp) {
		src_addr = solicited_addr;
		if (ifp->flags & IFA_F_OPTIMISTIC)
			override = false;
		inc_opt |= ifp->idev->cnf.force_tllao;
		in6_ifa_put(ifp);
	} else {
		if (ipv6_dev_get_saddr(dev_net(dev), dev, daddr,
				       inet6_sk(dev_net(dev)->ipv6.ndisc_sk)->srcprefs,
				       &tmpaddr))
			return;
		src_addr = &tmpaddr;
	}

	if (!dev->addr_len)
		inc_opt = 0;
	if (inc_opt)
		optlen += ndisc_opt_addr_space(dev);

	skb = ndisc_alloc_skb(dev, sizeof(*msg) + optlen);
	if (!skb)
		return;

	msg = (struct nd_msg *)skb_put(skb, sizeof(*msg));
	*msg = (struct nd_msg) {
		.icmph = {
			.icmp6_type = NDISC_NEIGHBOUR_ADVERTISEMENT,
			.icmp6_router = router,
			.icmp6_solicited = solicited,
			.icmp6_override = override,
		},
		.target = *solicited_addr,
	};

	if (inc_opt)
		ndisc_fill_addr_option(skb, ND_OPT_TARGET_LL_ADDR,
				       dev->dev_addr);


	ndisc_send_skb(skb, daddr, src_addr);
}
