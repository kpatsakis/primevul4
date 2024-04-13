static int __net_init sit_init_net(struct net *net)
{
	struct sit_net *sitn = net_generic(net, sit_net_id);
	int err;

	sitn->tunnels[0] = sitn->tunnels_wc;
	sitn->tunnels[1] = sitn->tunnels_l;
	sitn->tunnels[2] = sitn->tunnels_r;
	sitn->tunnels[3] = sitn->tunnels_r_l;

	sitn->fb_tunnel_dev = alloc_netdev(sizeof(struct ip_tunnel), "sit0",
					   ipip6_tunnel_setup);
	if (!sitn->fb_tunnel_dev) {
		err = -ENOMEM;
		goto err_alloc_dev;
	}
	dev_net_set(sitn->fb_tunnel_dev, net);

	err = ipip6_fb_tunnel_init(sitn->fb_tunnel_dev);
	if (err)
		goto err_dev_free;

	ipip6_tunnel_clone_6rd(sitn->fb_tunnel_dev, sitn);

	if ((err = register_netdev(sitn->fb_tunnel_dev)))
		goto err_reg_dev;

	return 0;

err_reg_dev:
	dev_put(sitn->fb_tunnel_dev);
err_dev_free:
	ipip6_dev_free(sitn->fb_tunnel_dev);
err_alloc_dev:
	return err;
}
