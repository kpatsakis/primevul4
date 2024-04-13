static void tcp_v6_restore_cb(struct sk_buff *skb)
{
	/* We need to move header back to the beginning if xfrm6_policy_check()
	 * and tcp_v6_fill_cb() are going to be called again.
	 * ip6_datagram_recv_specific_ctl() also expects IP6CB to be there.
	 */
	memmove(IP6CB(skb), &TCP_SKB_CB(skb)->header.h6,
		sizeof(struct inet6_skb_parm));
}
