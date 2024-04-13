static int __net_init ipgre_init_net(struct net *net)
{
	struct ipgre_net *ign = net_generic(net, ipgre_net_id);
	int err;

	ign->fb_tunnel_dev = alloc_netdev(sizeof(struct ip_tunnel), "gre0",
					   ipgre_tunnel_setup);
	if (!ign->fb_tunnel_dev) {
		err = -ENOMEM;
		goto err_alloc_dev;
	}
	dev_net_set(ign->fb_tunnel_dev, net);

	ipgre_fb_tunnel_init(ign->fb_tunnel_dev);
	ign->fb_tunnel_dev->rtnl_link_ops = &ipgre_link_ops;

	if ((err = register_netdev(ign->fb_tunnel_dev)))
		goto err_reg_dev;

	rcu_assign_pointer(ign->tunnels_wc[0],
			   netdev_priv(ign->fb_tunnel_dev));
	return 0;

err_reg_dev:
	ipgre_dev_free(ign->fb_tunnel_dev);
err_alloc_dev:
	return err;
}
