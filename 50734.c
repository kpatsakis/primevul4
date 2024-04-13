static void __net_exit tcp_sk_exit(struct net *net)
{
	int cpu;

	for_each_possible_cpu(cpu)
		inet_ctl_sock_destroy(*per_cpu_ptr(net->ipv4.tcp_sk, cpu));
	free_percpu(net->ipv4.tcp_sk);
}
