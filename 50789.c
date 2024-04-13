int __init tcpv6_init(void)
{
	int ret;

	ret = inet6_add_protocol(&tcpv6_protocol, IPPROTO_TCP);
	if (ret)
		goto out;

	/* register inet6 protocol */
	ret = inet6_register_protosw(&tcpv6_protosw);
	if (ret)
		goto out_tcpv6_protocol;

	ret = register_pernet_subsys(&tcpv6_net_ops);
	if (ret)
		goto out_tcpv6_protosw;
out:
	return ret;

out_tcpv6_protosw:
	inet6_unregister_protosw(&tcpv6_protosw);
out_tcpv6_protocol:
	inet6_del_protocol(&tcpv6_protocol, IPPROTO_TCP);
	goto out;
}
