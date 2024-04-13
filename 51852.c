static void tipc_sk_enqueue(struct sk_buff_head *inputq, struct sock *sk,
			    u32 dport)
{
	unsigned int lim;
	atomic_t *dcnt;
	struct sk_buff *skb;
	unsigned long time_limit = jiffies + 2;

	while (skb_queue_len(inputq)) {
		if (unlikely(time_after_eq(jiffies, time_limit)))
			return;

		skb = tipc_skb_dequeue(inputq, dport);
		if (unlikely(!skb))
			return;

		/* Add message directly to receive queue if possible */
		if (!sock_owned_by_user(sk)) {
			filter_rcv(sk, skb);
			continue;
		}

		/* Try backlog, compensating for double-counted bytes */
		dcnt = &tipc_sk(sk)->dupl_rcvcnt;
		if (!sk->sk_backlog.len)
			atomic_set(dcnt, 0);
		lim = rcvbuf_limit(sk, skb) + atomic_read(dcnt);
		if (likely(!sk_add_backlog(sk, skb, lim)))
			continue;

		/* Overload => reject message back to sender */
		tipc_sk_respond(sk, skb, TIPC_ERR_OVERLOAD);
		break;
	}
}
