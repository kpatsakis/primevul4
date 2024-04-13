static int ipgre_open(struct net_device *dev)
{
	struct ip_tunnel *t = netdev_priv(dev);

	if (ipv4_is_multicast(t->parms.iph.daddr)) {
		struct flowi fl = {
			.oif = t->parms.link,
			.fl4_dst = t->parms.iph.daddr,
			.fl4_src = t->parms.iph.saddr,
			.fl4_tos = RT_TOS(t->parms.iph.tos),
			.proto = IPPROTO_GRE,
			.fl_gre_key = t->parms.o_key
		};
		struct rtable *rt;

		if (ip_route_output_key(dev_net(dev), &rt, &fl))
			return -EADDRNOTAVAIL;
		dev = rt->dst.dev;
		ip_rt_put(rt);
		if (__in_dev_get_rtnl(dev) == NULL)
			return -EADDRNOTAVAIL;
		t->mlink = dev->ifindex;
		ip_mc_inc_group(__in_dev_get_rtnl(dev), t->parms.iph.daddr);
	}
	return 0;
}
