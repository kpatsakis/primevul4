static void __net_exit rtnetlink_net_exit(struct net *net)
{
	netlink_kernel_release(net->rtnl);
	net->rtnl = NULL;
}
