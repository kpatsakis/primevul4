static void __exit atalk_exit(void)
{
#ifdef CONFIG_SYSCTL
	atalk_unregister_sysctl();
#endif /* CONFIG_SYSCTL */
	atalk_proc_exit();
	aarp_cleanup_module();	/* General aarp clean-up. */
	unregister_netdevice_notifier(&ddp_notifier);
	dev_remove_pack(&ltalk_packet_type);
	dev_remove_pack(&ppptalk_packet_type);
	unregister_snap_client(ddp_dl);
	sock_unregister(PF_APPLETALK);
	proto_unregister(&ddp_proto);
}
