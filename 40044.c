static int __init bt_init(void)
{
	int err;

	BT_INFO("Core ver %s", VERSION);

	err = bt_sysfs_init();
	if (err < 0)
		return err;

	err = sock_register(&bt_sock_family_ops);
	if (err < 0) {
		bt_sysfs_cleanup();
		return err;
	}

	BT_INFO("HCI device and connection manager initialized");

	err = hci_sock_init();
	if (err < 0)
		goto error;

	err = l2cap_init();
	if (err < 0)
		goto sock_err;

	err = sco_init();
	if (err < 0) {
		l2cap_exit();
		goto sock_err;
	}

	return 0;

sock_err:
	hci_sock_cleanup();

error:
	sock_unregister(PF_BLUETOOTH);
	bt_sysfs_cleanup();

	return err;
}
