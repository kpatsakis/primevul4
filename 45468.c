static struct sco_conn *sco_conn_add(struct hci_conn *hcon)
{
	struct hci_dev *hdev = hcon->hdev;
	struct sco_conn *conn = hcon->sco_data;

	if (conn)
		return conn;

	conn = kzalloc(sizeof(struct sco_conn), GFP_KERNEL);
	if (!conn)
		return NULL;

	spin_lock_init(&conn->lock);

	hcon->sco_data = conn;
	conn->hcon = hcon;

	if (hdev->sco_mtu > 0)
		conn->mtu = hdev->sco_mtu;
	else
		conn->mtu = 60;

	BT_DBG("hcon %p conn %p", hcon, conn);

	return conn;
}
