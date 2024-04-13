static bool ieee80211_tx(struct ieee80211_sub_if_data *sdata,
			 struct sk_buff *skb, bool txpending,
			 enum ieee80211_band band)
{
	struct ieee80211_local *local = sdata->local;
	struct ieee80211_tx_data tx;
	ieee80211_tx_result res_prepare;
	struct ieee80211_tx_info *info = IEEE80211_SKB_CB(skb);
	bool result = true;
	int led_len;

	if (unlikely(skb->len < 10)) {
		dev_kfree_skb(skb);
		return true;
	}

	/* initialises tx */
	led_len = skb->len;
	res_prepare = ieee80211_tx_prepare(sdata, &tx, skb);

	if (unlikely(res_prepare == TX_DROP)) {
		ieee80211_free_txskb(&local->hw, skb);
		return true;
	} else if (unlikely(res_prepare == TX_QUEUED)) {
		return true;
	}

	info->band = band;

	/* set up hw_queue value early */
	if (!(info->flags & IEEE80211_TX_CTL_TX_OFFCHAN) ||
	    !(local->hw.flags & IEEE80211_HW_QUEUE_CONTROL))
		info->hw_queue =
			sdata->vif.hw_queue[skb_get_queue_mapping(skb)];

	if (!invoke_tx_handlers(&tx))
		result = __ieee80211_tx(local, &tx.skbs, led_len,
					tx.sta, txpending);

	return result;
}
