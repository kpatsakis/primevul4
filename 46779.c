static void flush_stack(struct sock **stack, unsigned int count,
			struct sk_buff *skb, unsigned int final)
{
	unsigned int i;
	struct sk_buff *skb1 = NULL;
	struct sock *sk;

	for (i = 0; i < count; i++) {
		sk = stack[i];
		if (likely(!skb1))
			skb1 = (i == final) ? skb : skb_clone(skb, GFP_ATOMIC);

		if (!skb1) {
			atomic_inc(&sk->sk_drops);
			UDP_INC_STATS_BH(sock_net(sk), UDP_MIB_RCVBUFERRORS,
					 IS_UDPLITE(sk));
			UDP_INC_STATS_BH(sock_net(sk), UDP_MIB_INERRORS,
					 IS_UDPLITE(sk));
		}

		if (skb1 && udp_queue_rcv_skb(sk, skb1) <= 0)
			skb1 = NULL;

		sock_put(sk);
	}
	if (unlikely(skb1))
		kfree_skb(skb1);
}
