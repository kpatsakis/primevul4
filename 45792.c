static void __exit x25_exit(void)
{
	x25_proc_exit();
	x25_link_free();
	x25_route_free();

	x25_unregister_sysctl();

	unregister_netdevice_notifier(&x25_dev_notifier);

	dev_remove_pack(&x25_packet_type);

	sock_unregister(AF_X25);
	proto_unregister(&x25_proto);
}
