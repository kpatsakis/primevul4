long unix_inq_len(struct sock *sk)
{
	struct sk_buff *skb;
	long amount = 0;

	if (sk->sk_state == TCP_LISTEN)
		return -EINVAL;

	spin_lock(&sk->sk_receive_queue.lock);
	if (sk->sk_type == SOCK_STREAM ||
	    sk->sk_type == SOCK_SEQPACKET) {
		skb_queue_walk(&sk->sk_receive_queue, skb)
			amount += unix_skb_len(skb);
	} else {
		skb = skb_peek(&sk->sk_receive_queue);
		if (skb)
			amount = skb->len;
	}
	spin_unlock(&sk->sk_receive_queue.lock);

	return amount;
}
