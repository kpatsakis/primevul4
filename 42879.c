static void ipip_tunnel_uninit(struct net_device *dev)
{
	struct net *net = dev_net(dev);
	struct ipip_net *ipn = net_generic(net, ipip_net_id);

	if (dev == ipn->fb_tunnel_dev)
		rcu_assign_pointer(ipn->tunnels_wc[0], NULL);
	else
		ipip_tunnel_unlink(ipn, netdev_priv(dev));
	dev_put(dev);
}
