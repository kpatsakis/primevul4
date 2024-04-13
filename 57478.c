static void __ip6_flush_pending_frames(struct sock *sk,
				       struct sk_buff_head *queue,
				       struct inet_cork_full *cork,
				       struct inet6_cork *v6_cork)
{
	struct sk_buff *skb;

	while ((skb = __skb_dequeue_tail(queue)) != NULL) {
		if (skb_dst(skb))
			IP6_INC_STATS(sock_net(sk), ip6_dst_idev(skb_dst(skb)),
				      IPSTATS_MIB_OUTDISCARDS);
		kfree_skb(skb);
	}

	ip6_cork_release(cork, v6_cork);
}
