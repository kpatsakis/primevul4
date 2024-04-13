static struct hci_conn *hidp_get_connection(struct hidp_session *session)
{
	bdaddr_t *src = &bt_sk(session->ctrl_sock->sk)->src;
	bdaddr_t *dst = &bt_sk(session->ctrl_sock->sk)->dst;
	struct hci_conn *conn;
	struct hci_dev *hdev;

	hdev = hci_get_route(dst, src);
	if (!hdev)
		return NULL;

	hci_dev_lock(hdev);
	conn = hci_conn_hash_lookup_ba(hdev, ACL_LINK, dst);
	if (conn)
		hci_conn_hold_device(conn);
	hci_dev_unlock(hdev);

	hci_dev_put(hdev);

	return conn;
}
