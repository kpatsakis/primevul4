static void ipip_tunnel_bind_dev(struct net_device *dev)
{
	struct net_device *tdev = NULL;
	struct ip_tunnel *tunnel;
	struct iphdr *iph;

	tunnel = netdev_priv(dev);
	iph = &tunnel->parms.iph;

	if (iph->daddr) {
		struct flowi fl = {
			.oif = tunnel->parms.link,
			.fl4_dst = iph->daddr,
			.fl4_src = iph->saddr,
			.fl4_tos = RT_TOS(iph->tos),
			.proto = IPPROTO_IPIP
		};
		struct rtable *rt;

		if (!ip_route_output_key(dev_net(dev), &rt, &fl)) {
			tdev = rt->dst.dev;
			ip_rt_put(rt);
		}
		dev->flags |= IFF_POINTOPOINT;
	}

	if (!tdev && tunnel->parms.link)
		tdev = __dev_get_by_index(dev_net(dev), tunnel->parms.link);

	if (tdev) {
		dev->hard_header_len = tdev->hard_header_len + sizeof(struct iphdr);
		dev->mtu = tdev->mtu - sizeof(struct iphdr);
	}
	dev->iflink = tunnel->parms.link;
}
