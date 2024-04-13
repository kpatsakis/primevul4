void tipc_sk_rcv(struct net *net, struct sk_buff_head *inputq)
{
	u32 dnode, dport = 0;
	int err;
	struct tipc_sock *tsk;
	struct sock *sk;
	struct sk_buff *skb;

	while (skb_queue_len(inputq)) {
		dport = tipc_skb_peek_port(inputq, dport);
		tsk = tipc_sk_lookup(net, dport);

		if (likely(tsk)) {
			sk = &tsk->sk;
			if (likely(spin_trylock_bh(&sk->sk_lock.slock))) {
				tipc_sk_enqueue(inputq, sk, dport);
				spin_unlock_bh(&sk->sk_lock.slock);
			}
			sock_put(sk);
			continue;
		}

		/* No destination socket => dequeue skb if still there */
		skb = tipc_skb_dequeue(inputq, dport);
		if (!skb)
			return;

		/* Try secondary lookup if unresolved named message */
		err = TIPC_ERR_NO_PORT;
		if (tipc_msg_lookup_dest(net, skb, &err))
			goto xmit;

		/* Prepare for message rejection */
		if (!tipc_msg_reverse(tipc_own_addr(net), &skb, err))
			continue;
xmit:
		dnode = msg_destnode(buf_msg(skb));
		tipc_node_xmit_skb(net, skb, dnode, dport);
	}
}
