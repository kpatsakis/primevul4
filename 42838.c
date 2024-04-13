static int ipgre_changelink(struct net_device *dev, struct nlattr *tb[],
			    struct nlattr *data[])
{
	struct ip_tunnel *t, *nt;
	struct net *net = dev_net(dev);
	struct ipgre_net *ign = net_generic(net, ipgre_net_id);
	struct ip_tunnel_parm p;
	int mtu;

	if (dev == ign->fb_tunnel_dev)
		return -EINVAL;

	nt = netdev_priv(dev);
	ipgre_netlink_parms(data, &p);

	t = ipgre_tunnel_locate(net, &p, 0);

	if (t) {
		if (t->dev != dev)
			return -EEXIST;
	} else {
		t = nt;

		if (dev->type != ARPHRD_ETHER) {
			unsigned int nflags = 0;

			if (ipv4_is_multicast(p.iph.daddr))
				nflags = IFF_BROADCAST;
			else if (p.iph.daddr)
				nflags = IFF_POINTOPOINT;

			if ((dev->flags ^ nflags) &
			    (IFF_POINTOPOINT | IFF_BROADCAST))
				return -EINVAL;
		}

		ipgre_tunnel_unlink(ign, t);
		t->parms.iph.saddr = p.iph.saddr;
		t->parms.iph.daddr = p.iph.daddr;
		t->parms.i_key = p.i_key;
		if (dev->type != ARPHRD_ETHER) {
			memcpy(dev->dev_addr, &p.iph.saddr, 4);
			memcpy(dev->broadcast, &p.iph.daddr, 4);
		}
		ipgre_tunnel_link(ign, t);
		netdev_state_change(dev);
	}

	t->parms.o_key = p.o_key;
	t->parms.iph.ttl = p.iph.ttl;
	t->parms.iph.tos = p.iph.tos;
	t->parms.iph.frag_off = p.iph.frag_off;

	if (t->parms.link != p.link) {
		t->parms.link = p.link;
		mtu = ipgre_tunnel_bind_dev(dev);
		if (!tb[IFLA_MTU])
			dev->mtu = mtu;
		netdev_state_change(dev);
	}

	return 0;
}
