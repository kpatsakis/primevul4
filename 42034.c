static void __net_exit diag_net_exit(struct net *net)
{
	netlink_kernel_release(net->diag_nlsk);
	net->diag_nlsk = NULL;
}
