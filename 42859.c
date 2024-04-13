static struct ip_tunnel *ipgre_tunnel_locate(struct net *net,
		struct ip_tunnel_parm *parms, int create)
{
	struct ip_tunnel *t, *nt;
	struct net_device *dev;
	char name[IFNAMSIZ];
	struct ipgre_net *ign = net_generic(net, ipgre_net_id);

	t = ipgre_tunnel_find(net, parms, ARPHRD_IPGRE);
	if (t || !create)
		return t;

	if (parms->name[0])
		strlcpy(name, parms->name, IFNAMSIZ);
	else
		strcpy(name, "gre%d");

	dev = alloc_netdev(sizeof(*t), name, ipgre_tunnel_setup);
	if (!dev)
		return NULL;

	dev_net_set(dev, net);

	if (strchr(name, '%')) {
		if (dev_alloc_name(dev, name) < 0)
			goto failed_free;
	}

	nt = netdev_priv(dev);
	nt->parms = *parms;
	dev->rtnl_link_ops = &ipgre_link_ops;

	dev->mtu = ipgre_tunnel_bind_dev(dev);

	if (register_netdevice(dev) < 0)
		goto failed_free;

	dev_hold(dev);
	ipgre_tunnel_link(ign, nt);
	return nt;

failed_free:
	free_netdev(dev);
	return NULL;
}
