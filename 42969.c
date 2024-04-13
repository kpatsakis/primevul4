static void ieee80211_tx_latency_start_msrmnt(struct ieee80211_local *local,
					      struct sk_buff *skb)
{
	struct timespec skb_arv;
	struct ieee80211_tx_latency_bin_ranges *tx_latency;

	tx_latency = rcu_dereference(local->tx_latency);
	if (!tx_latency)
		return;

	ktime_get_ts(&skb_arv);
	skb->tstamp = ktime_set(skb_arv.tv_sec, skb_arv.tv_nsec);
}
