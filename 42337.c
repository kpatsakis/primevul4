void hci_sock_cleanup(void)
{
	if (bt_sock_unregister(BTPROTO_HCI) < 0)
		BT_ERR("HCI socket unregistration failed");

	proto_unregister(&hci_sk_proto);
}
