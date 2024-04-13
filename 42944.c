void ieee80211_clear_tx_pending(struct ieee80211_local *local)
{
	struct sk_buff *skb;
	int i;

	for (i = 0; i < local->hw.queues; i++) {
		while ((skb = skb_dequeue(&local->pending[i])) != NULL)
			ieee80211_free_txskb(&local->hw, skb);
	}
}
