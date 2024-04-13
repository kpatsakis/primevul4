struct sk_buff *ieee80211_pspoll_get(struct ieee80211_hw *hw,
				     struct ieee80211_vif *vif)
{
	struct ieee80211_sub_if_data *sdata;
	struct ieee80211_if_managed *ifmgd;
	struct ieee80211_pspoll *pspoll;
	struct ieee80211_local *local;
	struct sk_buff *skb;

	if (WARN_ON(vif->type != NL80211_IFTYPE_STATION))
		return NULL;

	sdata = vif_to_sdata(vif);
	ifmgd = &sdata->u.mgd;
	local = sdata->local;

	skb = dev_alloc_skb(local->hw.extra_tx_headroom + sizeof(*pspoll));
	if (!skb)
		return NULL;

	skb_reserve(skb, local->hw.extra_tx_headroom);

	pspoll = (struct ieee80211_pspoll *) skb_put(skb, sizeof(*pspoll));
	memset(pspoll, 0, sizeof(*pspoll));
	pspoll->frame_control = cpu_to_le16(IEEE80211_FTYPE_CTL |
					    IEEE80211_STYPE_PSPOLL);
	pspoll->aid = cpu_to_le16(ifmgd->aid);

	/* aid in PS-Poll has its two MSBs each set to 1 */
	pspoll->aid |= cpu_to_le16(1 << 15 | 1 << 14);

	memcpy(pspoll->bssid, ifmgd->bssid, ETH_ALEN);
	memcpy(pspoll->ta, vif->addr, ETH_ALEN);

	return skb;
}
