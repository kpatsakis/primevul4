void ieee80211_xmit(struct ieee80211_sub_if_data *sdata, struct sk_buff *skb,
		    enum ieee80211_band band)
{
	struct ieee80211_local *local = sdata->local;
	struct ieee80211_tx_info *info = IEEE80211_SKB_CB(skb);
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *) skb->data;
	int headroom;
	bool may_encrypt;

	may_encrypt = !(info->flags & IEEE80211_TX_INTFL_DONT_ENCRYPT);

	headroom = local->tx_headroom;
	if (may_encrypt)
		headroom += sdata->encrypt_headroom;
	headroom -= skb_headroom(skb);
	headroom = max_t(int, 0, headroom);

	if (ieee80211_skb_resize(sdata, skb, headroom, may_encrypt)) {
		ieee80211_free_txskb(&local->hw, skb);
		return;
	}

	hdr = (struct ieee80211_hdr *) skb->data;
	info->control.vif = &sdata->vif;

	if (ieee80211_vif_is_mesh(&sdata->vif)) {
		if (ieee80211_is_data(hdr->frame_control) &&
		    is_unicast_ether_addr(hdr->addr1)) {
			if (mesh_nexthop_resolve(sdata, skb))
				return; /* skb queued: don't free */
		} else {
			ieee80211_mps_set_frame_flags(sdata, NULL, hdr);
		}
	}

	ieee80211_set_qos_hdr(sdata, skb);
	ieee80211_tx(sdata, skb, false, band);
}
