static void __exit ipgre_fini(void)
{
	rtnl_link_unregister(&ipgre_tap_ops);
	rtnl_link_unregister(&ipgre_link_ops);
	if (gre_del_protocol(&ipgre_protocol, GREPROTO_CISCO) < 0)
		printk(KERN_INFO "ipgre close: can't remove protocol\n");
	unregister_pernet_device(&ipgre_net_ops);
}
