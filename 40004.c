static void sco_recv_frame(struct sco_conn *conn, struct sk_buff *skb)
{
	struct sock *sk = sco_chan_get(conn);

	if (!sk)
		goto drop;

	BT_DBG("sk %p len %d", sk, skb->len);

	if (sk->sk_state != BT_CONNECTED)
		goto drop;

	if (!sock_queue_rcv_skb(sk, skb))
		return;

drop:
	kfree_skb(skb);
}
