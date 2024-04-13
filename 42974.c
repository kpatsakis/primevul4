bool ieee80211_tx_prepare_skb(struct ieee80211_hw *hw,
			      struct ieee80211_vif *vif, struct sk_buff *skb,
			      int band, struct ieee80211_sta **sta)
{
	struct ieee80211_sub_if_data *sdata = vif_to_sdata(vif);
	struct ieee80211_tx_info *info = IEEE80211_SKB_CB(skb);
	struct ieee80211_tx_data tx;

	if (ieee80211_tx_prepare(sdata, &tx, skb) == TX_DROP)
		return false;

	info->band = band;
	info->control.vif = vif;
	info->hw_queue = vif->hw_queue[skb_get_queue_mapping(skb)];

	if (invoke_tx_handlers(&tx))
		return false;

	if (sta) {
		if (tx.sta)
			*sta = &tx.sta->sta;
		else
			*sta = NULL;
	}

	return true;
}
