static void __exit pppoe_exit(void)
{
	unregister_netdevice_notifier(&pppoe_notifier);
	dev_remove_pack(&pppoed_ptype);
	dev_remove_pack(&pppoes_ptype);
	unregister_pppox_proto(PX_PROTO_OE);
	proto_unregister(&pppoe_sk_proto);
	unregister_pernet_device(&pppoe_net_ops);
}
