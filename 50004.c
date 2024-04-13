static void __net_exit icmpv6_sk_exit(struct net *net)
{
	int i;

	for_each_possible_cpu(i) {
		inet_ctl_sock_destroy(net->ipv6.icmp_sk[i]);
	}
	kfree(net->ipv6.icmp_sk);
}
