static void __hidp_unlink_session(struct hidp_session *session)
{
	hci_conn_put_device(session->conn);

	list_del(&session->list);
}
