void sco_disconn_cfm(struct hci_conn *hcon, __u8 reason)
{
	BT_DBG("hcon %p reason %d", hcon, reason);

	sco_conn_del(hcon, bt_to_errno(reason));
}
