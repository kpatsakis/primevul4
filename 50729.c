bool tcp_prequeue(struct sock *sk, struct sk_buff *skb)
{
	struct tcp_sock *tp = tcp_sk(sk);

	if (sysctl_tcp_low_latency || !tp->ucopy.task)
		return false;

	if (skb->len <= tcp_hdrlen(skb) &&
	    skb_queue_len(&tp->ucopy.prequeue) == 0)
		return false;

	/* Before escaping RCU protected region, we need to take care of skb
	 * dst. Prequeue is only enabled for established sockets.
	 * For such sockets, we might need the skb dst only to set sk->sk_rx_dst
	 * Instead of doing full sk_rx_dst validity here, let's perform
	 * an optimistic check.
	 */
	if (likely(sk->sk_rx_dst))
		skb_dst_drop(skb);
	else
		skb_dst_force_safe(skb);

	__skb_queue_tail(&tp->ucopy.prequeue, skb);
	tp->ucopy.memory += skb->truesize;
	if (skb_queue_len(&tp->ucopy.prequeue) >= 32 ||
	    tp->ucopy.memory + atomic_read(&sk->sk_rmem_alloc) > sk->sk_rcvbuf) {
		struct sk_buff *skb1;

		BUG_ON(sock_owned_by_user(sk));
		__NET_ADD_STATS(sock_net(sk), LINUX_MIB_TCPPREQUEUEDROPPED,
				skb_queue_len(&tp->ucopy.prequeue));

		while ((skb1 = __skb_dequeue(&tp->ucopy.prequeue)) != NULL)
			sk_backlog_rcv(sk, skb1);

		tp->ucopy.memory = 0;
	} else if (skb_queue_len(&tp->ucopy.prequeue) == 1) {
		wake_up_interruptible_sync_poll(sk_sleep(sk),
					   POLLIN | POLLRDNORM | POLLRDBAND);
		if (!inet_csk_ack_scheduled(sk))
			inet_csk_reset_xmit_timer(sk, ICSK_TIME_DACK,
						  (3 * tcp_rto_min(sk)) / 4,
						  TCP_RTO_MAX);
	}
	return true;
}
