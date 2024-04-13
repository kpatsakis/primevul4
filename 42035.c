static int __net_init diag_net_init(struct net *net)
{
	struct netlink_kernel_cfg cfg = {
		.input	= sock_diag_rcv,
	};

	net->diag_nlsk = netlink_kernel_create(net, NETLINK_SOCK_DIAG, &cfg);
	return net->diag_nlsk == NULL ? -ENOMEM : 0;
}
