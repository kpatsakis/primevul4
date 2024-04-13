void skb_orphan_partial(struct sk_buff *skb)
{
	/* If this skb is a TCP pure ACK or already went here,
	 * we have nothing to do. 2 is already a very small truesize.
	 */
	if (skb->truesize <= 2)
		return;

	/* TCP stack sets skb->ooo_okay based on sk_wmem_alloc,
	 * so we do not completely orphan skb, but transfert all
	 * accounted bytes but one, to avoid unexpected reorders.
	 */
	if (skb->destructor == sock_wfree
#ifdef CONFIG_INET
	    || skb->destructor == tcp_wfree
#endif
		) {
		atomic_sub(skb->truesize - 1, &skb->sk->sk_wmem_alloc);
		skb->truesize = 1;
	} else {
		skb_orphan(skb);
	}
}
