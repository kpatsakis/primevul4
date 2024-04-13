static void __exit bt_exit(void)
{

	sco_exit();

	l2cap_exit();

	hci_sock_cleanup();

	sock_unregister(PF_BLUETOOTH);

	bt_sysfs_cleanup();
}
