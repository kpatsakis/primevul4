static int __init ipgre_init(void)
{
	int err;

	printk(KERN_INFO "GRE over IPv4 tunneling driver\n");

	err = register_pernet_device(&ipgre_net_ops);
	if (err < 0)
		return err;

	err = gre_add_protocol(&ipgre_protocol, GREPROTO_CISCO);
	if (err < 0) {
		printk(KERN_INFO "ipgre init: can't add protocol\n");
		goto add_proto_failed;
	}

	err = rtnl_link_register(&ipgre_link_ops);
	if (err < 0)
		goto rtnl_link_failed;

	err = rtnl_link_register(&ipgre_tap_ops);
	if (err < 0)
		goto tap_ops_failed;

out:
	return err;

tap_ops_failed:
	rtnl_link_unregister(&ipgre_link_ops);
rtnl_link_failed:
	gre_del_protocol(&ipgre_protocol, GREPROTO_CISCO);
add_proto_failed:
	unregister_pernet_device(&ipgre_net_ops);
	goto out;
}
