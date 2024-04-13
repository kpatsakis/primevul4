void icmpv6_cleanup(void)
{
	inet6_unregister_icmp_sender(icmp6_send);
	unregister_pernet_subsys(&icmpv6_sk_ops);
	inet6_del_protocol(&icmpv6_protocol, IPPROTO_ICMPV6);
}
