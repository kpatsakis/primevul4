int sock_queue_rcv_skb(struct sock *sk, struct sk_buff *skb)
{
	int err;

	err = sk_filter(sk, skb);
	if (err)
		return err;

	return __sock_queue_rcv_skb(sk, skb);
}
