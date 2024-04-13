static void __net_exit ndisc_net_exit(struct net *net)
{
	inet_ctl_sock_destroy(net->ipv6.ndisc_sk);
}
