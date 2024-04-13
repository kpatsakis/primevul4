static int __net_init ipip_init_net(struct net *net)
{
	struct ipip_net *ipn = net_generic(net, ipip_net_id);
	int err;

	ipn->tunnels[0] = ipn->tunnels_wc;
	ipn->tunnels[1] = ipn->tunnels_l;
	ipn->tunnels[2] = ipn->tunnels_r;
	ipn->tunnels[3] = ipn->tunnels_r_l;

	ipn->fb_tunnel_dev = alloc_netdev(sizeof(struct ip_tunnel),
					   "tunl0",
					   ipip_tunnel_setup);
	if (!ipn->fb_tunnel_dev) {
		err = -ENOMEM;
		goto err_alloc_dev;
	}
	dev_net_set(ipn->fb_tunnel_dev, net);

	err = ipip_fb_tunnel_init(ipn->fb_tunnel_dev);
	if (err)
		goto err_reg_dev;

	if ((err = register_netdev(ipn->fb_tunnel_dev)))
		goto err_reg_dev;

	return 0;

err_reg_dev:
	ipip_dev_free(ipn->fb_tunnel_dev);
err_alloc_dev:
	/* nothing */
	return err;
}
