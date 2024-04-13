static int __init pppoe_init(void)
{
	int err;

	err = register_pernet_device(&pppoe_net_ops);
	if (err)
		goto out;

	err = proto_register(&pppoe_sk_proto, 0);
	if (err)
		goto out_unregister_net_ops;

	err = register_pppox_proto(PX_PROTO_OE, &pppoe_proto);
	if (err)
		goto out_unregister_pppoe_proto;

	dev_add_pack(&pppoes_ptype);
	dev_add_pack(&pppoed_ptype);
	register_netdevice_notifier(&pppoe_notifier);

	return 0;

out_unregister_pppoe_proto:
	proto_unregister(&pppoe_sk_proto);
out_unregister_net_ops:
	unregister_pernet_device(&pppoe_net_ops);
out:
	return err;
}
