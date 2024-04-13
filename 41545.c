static int tg3_tso_bug(struct tg3 *tp, struct sk_buff *skb)
{
	struct sk_buff *segs, *nskb;
	u32 frag_cnt_est = skb_shinfo(skb)->gso_segs * 3;

	/* Estimate the number of fragments in the worst case */
	if (unlikely(tg3_tx_avail(&tp->napi[0]) <= frag_cnt_est)) {
		netif_stop_queue(tp->dev);

		/* netif_tx_stop_queue() must be done before checking
		 * checking tx index in tg3_tx_avail() below, because in
		 * tg3_tx(), we update tx index before checking for
		 * netif_tx_queue_stopped().
		 */
		smp_mb();
		if (tg3_tx_avail(&tp->napi[0]) <= frag_cnt_est)
			return NETDEV_TX_BUSY;

		netif_wake_queue(tp->dev);
	}

	segs = skb_gso_segment(skb, tp->dev->features & ~NETIF_F_TSO);
	if (IS_ERR(segs))
		goto tg3_tso_bug_end;

	do {
		nskb = segs;
		segs = segs->next;
		nskb->next = NULL;
		tg3_start_xmit(nskb, tp->dev);
	} while (segs);

tg3_tso_bug_end:
	dev_kfree_skb(skb);

	return NETDEV_TX_OK;
}
