static int __net_init xfrm_user_net_init(struct net *net)
{
	struct sock *nlsk;
	struct netlink_kernel_cfg cfg = {
		.groups	= XFRMNLGRP_MAX,
		.input	= xfrm_netlink_rcv,
	};

	nlsk = netlink_kernel_create(net, NETLINK_XFRM, THIS_MODULE, &cfg);
	if (nlsk == NULL)
		return -ENOMEM;
	net->xfrm.nlsk_stash = nlsk; /* Don't set to NULL */
	rcu_assign_pointer(net->xfrm.nlsk, nlsk);
	return 0;
}
