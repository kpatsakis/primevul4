void __exit rfcomm_cleanup_sockets(void)
{
	bt_procfs_cleanup(&init_net, "rfcomm");

	debugfs_remove(rfcomm_sock_debugfs);

	if (bt_sock_unregister(BTPROTO_RFCOMM) < 0)
		BT_ERR("RFCOMM socket layer unregistration failed");

	proto_unregister(&rfcomm_proto);
}
