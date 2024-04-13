static int ipgre_newlink(struct net *src_net, struct net_device *dev, struct nlattr *tb[],
			 struct nlattr *data[])
{
	struct ip_tunnel *nt;
	struct net *net = dev_net(dev);
	struct ipgre_net *ign = net_generic(net, ipgre_net_id);
	int mtu;
	int err;

	nt = netdev_priv(dev);
	ipgre_netlink_parms(data, &nt->parms);

	if (ipgre_tunnel_find(net, &nt->parms, dev->type))
		return -EEXIST;

	if (dev->type == ARPHRD_ETHER && !tb[IFLA_ADDRESS])
		random_ether_addr(dev->dev_addr);

	mtu = ipgre_tunnel_bind_dev(dev);
	if (!tb[IFLA_MTU])
		dev->mtu = mtu;

	/* Can use a lockless transmit, unless we generate output sequences */
	if (!(nt->parms.o_flags & GRE_SEQ))
		dev->features |= NETIF_F_LLTX;

	err = register_netdevice(dev);
	if (err)
		goto out;

	dev_hold(dev);
	ipgre_tunnel_link(ign, nt);

out:
	return err;
}
