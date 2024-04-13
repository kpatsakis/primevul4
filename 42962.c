ieee80211_tx_h_fragment(struct ieee80211_tx_data *tx)
{
	struct sk_buff *skb = tx->skb;
	struct ieee80211_tx_info *info = IEEE80211_SKB_CB(skb);
	struct ieee80211_hdr *hdr = (void *)skb->data;
	int frag_threshold = tx->local->hw.wiphy->frag_threshold;
	int hdrlen;
	int fragnum;

	/* no matter what happens, tx->skb moves to tx->skbs */
	__skb_queue_tail(&tx->skbs, skb);
	tx->skb = NULL;

	if (info->flags & IEEE80211_TX_CTL_DONTFRAG)
		return TX_CONTINUE;

	if (tx->local->ops->set_frag_threshold)
		return TX_CONTINUE;

	/*
	 * Warn when submitting a fragmented A-MPDU frame and drop it.
	 * This scenario is handled in ieee80211_tx_prepare but extra
	 * caution taken here as fragmented ampdu may cause Tx stop.
	 */
	if (WARN_ON(info->flags & IEEE80211_TX_CTL_AMPDU))
		return TX_DROP;

	hdrlen = ieee80211_hdrlen(hdr->frame_control);

	/* internal error, why isn't DONTFRAG set? */
	if (WARN_ON(skb->len + FCS_LEN <= frag_threshold))
		return TX_DROP;

	/*
	 * Now fragment the frame. This will allocate all the fragments and
	 * chain them (using skb as the first fragment) to skb->next.
	 * During transmission, we will remove the successfully transmitted
	 * fragments from this list. When the low-level driver rejects one
	 * of the fragments then we will simply pretend to accept the skb
	 * but store it away as pending.
	 */
	if (ieee80211_fragment(tx, skb, hdrlen, frag_threshold))
		return TX_DROP;

	/* update duration/seq/flags of fragments */
	fragnum = 0;

	skb_queue_walk(&tx->skbs, skb) {
		const __le16 morefrags = cpu_to_le16(IEEE80211_FCTL_MOREFRAGS);

		hdr = (void *)skb->data;
		info = IEEE80211_SKB_CB(skb);

		if (!skb_queue_is_last(&tx->skbs, skb)) {
			hdr->frame_control |= morefrags;
			/*
			 * No multi-rate retries for fragmented frames, that
			 * would completely throw off the NAV at other STAs.
			 */
			info->control.rates[1].idx = -1;
			info->control.rates[2].idx = -1;
			info->control.rates[3].idx = -1;
			BUILD_BUG_ON(IEEE80211_TX_MAX_RATES != 4);
			info->flags &= ~IEEE80211_TX_CTL_RATE_CTRL_PROBE;
		} else {
			hdr->frame_control &= ~morefrags;
		}
		hdr->seq_ctrl |= cpu_to_le16(fragnum & IEEE80211_SCTL_FRAG);
		fragnum++;
	}

	return TX_CONTINUE;
}
