static inline struct sock *icmpv6_sk(struct net *net)
{
	return net->ipv6.icmp_sk[smp_processor_id()];
}
