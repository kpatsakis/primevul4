static int pn_backlog_rcv(struct sock *sk, struct sk_buff *skb)
{
	int err = sock_queue_rcv_skb(sk, skb);

	if (err < 0)
		kfree_skb(skb);
	return err ? NET_RX_DROP : NET_RX_SUCCESS;
}
