void __exit sco_exit(void)
{
	bt_procfs_cleanup(&init_net, "sco");

	debugfs_remove(sco_debugfs);

	bt_sock_unregister(BTPROTO_SCO);

	proto_unregister(&sco_proto);
}
