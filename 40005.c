int sco_recv_scodata(struct hci_conn *hcon, struct sk_buff *skb)
{
	struct sco_conn *conn = hcon->sco_data;

	if (!conn)
		goto drop;

	BT_DBG("conn %p len %d", conn, skb->len);

	if (skb->len) {
		sco_recv_frame(conn, skb);
		return 0;
	}

drop:
	kfree_skb(skb);
	return 0;
}
