static int __net_init nfnetlink_net_init(struct net *net)
{
	struct sock *nfnl;
	struct netlink_kernel_cfg cfg = {
		.groups	= NFNLGRP_MAX,
		.input	= nfnetlink_rcv,
#ifdef CONFIG_MODULES
		.bind	= nfnetlink_bind,
#endif
	};

	nfnl = netlink_kernel_create(net, NETLINK_NETFILTER, &cfg);
	if (!nfnl)
		return -ENOMEM;
	net->nfnl_stash = nfnl;
	rcu_assign_pointer(net->nfnl, nfnl);
	return 0;
}
