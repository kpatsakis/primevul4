int __init hci_sock_init(void)
{
	int err;

	err = proto_register(&hci_sk_proto, 0);
	if (err < 0)
		return err;

	err = bt_sock_register(BTPROTO_HCI, &hci_sock_family_ops);
	if (err < 0) {
		BT_ERR("HCI socket registration failed");
		goto error;
	}

	err = bt_procfs_init(&init_net, "hci", &hci_sk_list, NULL);
	if (err < 0) {
		BT_ERR("Failed to create HCI proc file");
		bt_sock_unregister(BTPROTO_HCI);
		goto error;
	}

	BT_INFO("HCI socket layer initialized");

	return 0;

error:
	proto_unregister(&hci_sk_proto);
	return err;
}
