ieee80211_tx_h_stats(struct ieee80211_tx_data *tx)
{
	struct sk_buff *skb;
	int ac = -1;

	if (!tx->sta)
		return TX_CONTINUE;

	skb_queue_walk(&tx->skbs, skb) {
		ac = skb_get_queue_mapping(skb);
		tx->sta->tx_fragments++;
		tx->sta->tx_bytes[ac] += skb->len;
	}
	if (ac >= 0)
		tx->sta->tx_packets[ac]++;

	return TX_CONTINUE;
}
