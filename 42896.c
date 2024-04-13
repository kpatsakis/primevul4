static void ipip6_tunnel_uninit(struct net_device *dev)
{
	struct net *net = dev_net(dev);
	struct sit_net *sitn = net_generic(net, sit_net_id);

	if (dev == sitn->fb_tunnel_dev) {
		rcu_assign_pointer(sitn->tunnels_wc[0], NULL);
	} else {
		ipip6_tunnel_unlink(sitn, netdev_priv(dev));
		ipip6_tunnel_del_prl(netdev_priv(dev), NULL);
	}
	dev_put(dev);
}
