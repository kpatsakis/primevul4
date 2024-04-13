ieee80211_tx_h_calculate_duration(struct ieee80211_tx_data *tx)
{
	struct sk_buff *skb;
	struct ieee80211_hdr *hdr;
	int next_len;
	bool group_addr;

	skb_queue_walk(&tx->skbs, skb) {
		hdr = (void *) skb->data;
		if (unlikely(ieee80211_is_pspoll(hdr->frame_control)))
			break; /* must not overwrite AID */
		if (!skb_queue_is_last(&tx->skbs, skb)) {
			struct sk_buff *next = skb_queue_next(&tx->skbs, skb);
			next_len = next->len;
		} else
			next_len = 0;
		group_addr = is_multicast_ether_addr(hdr->addr1);

		hdr->duration_id =
			ieee80211_duration(tx, skb, group_addr, next_len);
	}

	return TX_CONTINUE;
}
