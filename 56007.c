static __net_init int sysctl_route_net_init(struct net *net)
{
	struct ctl_table *tbl;

	tbl = ipv4_route_flush_table;
	if (!net_eq(net, &init_net)) {
		tbl = kmemdup(tbl, sizeof(ipv4_route_flush_table), GFP_KERNEL);
		if (!tbl)
			goto err_dup;

		/* Don't export sysctls to unprivileged users */
		if (net->user_ns != &init_user_ns)
			tbl[0].procname = NULL;
	}
	tbl[0].extra1 = net;

	net->ipv4.route_hdr = register_net_sysctl(net, "net/ipv4/route", tbl);
	if (!net->ipv4.route_hdr)
		goto err_reg;
	return 0;

err_reg:
	if (tbl != ipv4_route_flush_table)
		kfree(tbl);
err_dup:
	return -ENOMEM;
}
