void skb_condense(struct sk_buff *skb)
{
	if (skb->data_len) {
		if (skb->data_len > skb->end - skb->tail ||
		    skb_cloned(skb))
			return;

		/* Nice, we can free page frag(s) right now */
		__pskb_pull_tail(skb, skb->data_len);
	}
	/* At this point, skb->truesize might be over estimated,
	 * because skb had a fragment, and fragments do not tell
	 * their truesize.
	 * When we pulled its content into skb->head, fragment
	 * was freed, but __pskb_pull_tail() could not possibly
	 * adjust skb->truesize, not knowing the frag truesize.
	 */
	skb->truesize = SKB_TRUESIZE(skb_end_offset(skb));
}
