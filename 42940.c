static bool __ieee80211_tx(struct ieee80211_local *local,
			   struct sk_buff_head *skbs, int led_len,
			   struct sta_info *sta, bool txpending)
{
	struct ieee80211_tx_info *info;
	struct ieee80211_sub_if_data *sdata;
	struct ieee80211_vif *vif;
	struct ieee80211_sta *pubsta;
	struct sk_buff *skb;
	bool result = true;
	__le16 fc;

	if (WARN_ON(skb_queue_empty(skbs)))
		return true;

	skb = skb_peek(skbs);
	fc = ((struct ieee80211_hdr *)skb->data)->frame_control;
	info = IEEE80211_SKB_CB(skb);
	sdata = vif_to_sdata(info->control.vif);
	if (sta && !sta->uploaded)
		sta = NULL;

	if (sta)
		pubsta = &sta->sta;
	else
		pubsta = NULL;

	switch (sdata->vif.type) {
	case NL80211_IFTYPE_MONITOR:
		if (sdata->u.mntr_flags & MONITOR_FLAG_ACTIVE) {
			vif = &sdata->vif;
			break;
		}
		sdata = rcu_dereference(local->monitor_sdata);
		if (sdata) {
			vif = &sdata->vif;
			info->hw_queue =
				vif->hw_queue[skb_get_queue_mapping(skb)];
		} else if (local->hw.flags & IEEE80211_HW_QUEUE_CONTROL) {
			dev_kfree_skb(skb);
			return true;
		} else
			vif = NULL;
		break;
	case NL80211_IFTYPE_AP_VLAN:
		sdata = container_of(sdata->bss,
				     struct ieee80211_sub_if_data, u.ap);
		/* fall through */
	default:
		vif = &sdata->vif;
		break;
	}

	result = ieee80211_tx_frags(local, vif, pubsta, skbs,
				    txpending);

	ieee80211_tpt_led_trig_tx(local, fc, led_len);

	WARN_ON_ONCE(!skb_queue_empty(skbs));

	return result;
}
