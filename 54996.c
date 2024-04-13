static struct device *bnep_get_device(struct bnep_session *session)
{
	struct hci_conn *conn;

	conn = l2cap_pi(session->sock->sk)->chan->conn->hcon;
	if (!conn)
		return NULL;

	return &conn->dev;
}
