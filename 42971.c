static bool ieee80211_tx_pending_skb(struct ieee80211_local *local,
				     struct sk_buff *skb)
{
	struct ieee80211_tx_info *info = IEEE80211_SKB_CB(skb);
	struct ieee80211_sub_if_data *sdata;
	struct sta_info *sta;
	struct ieee80211_hdr *hdr;
	bool result;
	struct ieee80211_chanctx_conf *chanctx_conf;

	sdata = vif_to_sdata(info->control.vif);

	if (info->flags & IEEE80211_TX_INTFL_NEED_TXPROCESSING) {
		chanctx_conf = rcu_dereference(sdata->vif.chanctx_conf);
		if (unlikely(!chanctx_conf)) {
			dev_kfree_skb(skb);
			return true;
		}
		result = ieee80211_tx(sdata, skb, true,
				      chanctx_conf->def.chan->band);
	} else {
		struct sk_buff_head skbs;

		__skb_queue_head_init(&skbs);
		__skb_queue_tail(&skbs, skb);

		hdr = (struct ieee80211_hdr *)skb->data;
		sta = sta_info_get(sdata, hdr->addr1);

		result = __ieee80211_tx(local, &skbs, skb->len, sta, true);
	}

	return result;
}
