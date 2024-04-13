void __exit sco_exit(void)
{
	bt_procfs_cleanup(&init_net, "sco");

	debugfs_remove(sco_debugfs);

	if (bt_sock_unregister(BTPROTO_SCO) < 0)
		BT_ERR("SCO socket unregistration failed");

	proto_unregister(&sco_proto);
}
