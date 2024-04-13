static int __net_init icmpv6_sk_init(struct net *net)
{
	struct sock *sk;
	int err, i, j;

	net->ipv6.icmp_sk =
		kzalloc(nr_cpu_ids * sizeof(struct sock *), GFP_KERNEL);
	if (!net->ipv6.icmp_sk)
		return -ENOMEM;

	for_each_possible_cpu(i) {
		err = inet_ctl_sock_create(&sk, PF_INET6,
					   SOCK_RAW, IPPROTO_ICMPV6, net);
		if (err < 0) {
			pr_err("Failed to initialize the ICMP6 control socket (err %d)\n",
			       err);
			goto fail;
		}

		net->ipv6.icmp_sk[i] = sk;

		/* Enough space for 2 64K ICMP packets, including
		 * sk_buff struct overhead.
		 */
		sk->sk_sndbuf = 2 * SKB_TRUESIZE(64 * 1024);
	}
	return 0;

 fail:
	for (j = 0; j < i; j++)
		inet_ctl_sock_destroy(net->ipv6.icmp_sk[j]);
	kfree(net->ipv6.icmp_sk);
	return err;
}
