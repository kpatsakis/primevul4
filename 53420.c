static void rfcomm_sk_data_ready(struct rfcomm_dlc *d, struct sk_buff *skb)
{
	struct sock *sk = d->owner;
	if (!sk)
		return;

	atomic_add(skb->len, &sk->sk_rmem_alloc);
	skb_queue_tail(&sk->sk_receive_queue, skb);
	sk->sk_data_ready(sk);

	if (atomic_read(&sk->sk_rmem_alloc) >= sk->sk_rcvbuf)
		rfcomm_dlc_throttle(d);
}
