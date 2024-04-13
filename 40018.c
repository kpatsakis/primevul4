int __init rfcomm_init_sockets(void)
{
	int err;

	err = proto_register(&rfcomm_proto, 0);
	if (err < 0)
		return err;

	err = bt_sock_register(BTPROTO_RFCOMM, &rfcomm_sock_family_ops);
	if (err < 0) {
		BT_ERR("RFCOMM socket layer registration failed");
		goto error;
	}

	err = bt_procfs_init(THIS_MODULE, &init_net, "rfcomm", &rfcomm_sk_list, NULL);
	if (err < 0) {
		BT_ERR("Failed to create RFCOMM proc file");
		bt_sock_unregister(BTPROTO_RFCOMM);
		goto error;
	}

	if (bt_debugfs) {
		rfcomm_sock_debugfs = debugfs_create_file("rfcomm", 0444,
				bt_debugfs, NULL, &rfcomm_sock_debugfs_fops);
		if (!rfcomm_sock_debugfs)
			BT_ERR("Failed to create RFCOMM debug file");
	}

	BT_INFO("RFCOMM socket layer initialized");

	return 0;

error:
	proto_unregister(&rfcomm_proto);
	return err;
}
