int __init icmpv6_init(void)
{
	int err;

	err = register_pernet_subsys(&icmpv6_sk_ops);
	if (err < 0)
		return err;

	err = -EAGAIN;
	if (inet6_add_protocol(&icmpv6_protocol, IPPROTO_ICMPV6) < 0)
		goto fail;

	err = inet6_register_icmp_sender(icmp6_send);
	if (err)
		goto sender_reg_err;
	return 0;

sender_reg_err:
	inet6_del_protocol(&icmpv6_protocol, IPPROTO_ICMPV6);
fail:
	pr_err("Failed to register ICMP6 protocol\n");
	unregister_pernet_subsys(&icmpv6_sk_ops);
	return err;
}
