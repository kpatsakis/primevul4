int sock_queue_rcv_skb(struct sock *sk, struct sk_buff *skb)
{
	int err;
	int skb_len;
	unsigned long flags;
	struct sk_buff_head *list = &sk->sk_receive_queue;

	if (atomic_read(&sk->sk_rmem_alloc) >= sk->sk_rcvbuf) {
		atomic_inc(&sk->sk_drops);
		trace_sock_rcvqueue_full(sk, skb);
		return -ENOMEM;
	}

	err = sk_filter(sk, skb);
	if (err)
		return err;

	if (!sk_rmem_schedule(sk, skb, skb->truesize)) {
		atomic_inc(&sk->sk_drops);
		return -ENOBUFS;
	}

	skb->dev = NULL;
	skb_set_owner_r(skb, sk);

	/* Cache the SKB length before we tack it onto the receive
	 * queue.  Once it is added it no longer belongs to us and
	 * may be freed by other threads of control pulling packets
	 * from the queue.
	 */
	skb_len = skb->len;

	/* we escape from rcu protected region, make sure we dont leak
	 * a norefcounted dst
	 */
	skb_dst_force(skb);

	spin_lock_irqsave(&list->lock, flags);
	skb->dropcount = atomic_read(&sk->sk_drops);
	__skb_queue_tail(list, skb);
	spin_unlock_irqrestore(&list->lock, flags);

	if (!sock_flag(sk, SOCK_DEAD))
		sk->sk_data_ready(sk, skb_len);
	return 0;
}
