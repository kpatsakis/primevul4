static void __exit ax25_exit(void)
{
	remove_proc_entry("ax25_route", init_net.proc_net);
	remove_proc_entry("ax25", init_net.proc_net);
	remove_proc_entry("ax25_calls", init_net.proc_net);

	unregister_netdevice_notifier(&ax25_dev_notifier);

	dev_remove_pack(&ax25_packet_type);

	sock_unregister(PF_AX25);
	proto_unregister(&ax25_proto);

	ax25_rt_free();
	ax25_uid_free();
	ax25_dev_free();
}
