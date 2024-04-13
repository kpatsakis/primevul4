ieee80211_tx_h_check_assoc(struct ieee80211_tx_data *tx)
{

	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *)tx->skb->data;
	struct ieee80211_tx_info *info = IEEE80211_SKB_CB(tx->skb);
	bool assoc = false;

	if (unlikely(info->flags & IEEE80211_TX_CTL_INJECTED))
		return TX_CONTINUE;

	if (unlikely(test_bit(SCAN_SW_SCANNING, &tx->local->scanning)) &&
	    test_bit(SDATA_STATE_OFFCHANNEL, &tx->sdata->state) &&
	    !ieee80211_is_probe_req(hdr->frame_control) &&
	    !ieee80211_is_nullfunc(hdr->frame_control))
		/*
		 * When software scanning only nullfunc frames (to notify
		 * the sleep state to the AP) and probe requests (for the
		 * active scan) are allowed, all other frames should not be
		 * sent and we should not get here, but if we do
		 * nonetheless, drop them to avoid sending them
		 * off-channel. See the link below and
		 * ieee80211_start_scan() for more.
		 *
		 * http://article.gmane.org/gmane.linux.kernel.wireless.general/30089
		 */
		return TX_DROP;

	if (tx->sdata->vif.type == NL80211_IFTYPE_WDS)
		return TX_CONTINUE;

	if (tx->sdata->vif.type == NL80211_IFTYPE_MESH_POINT)
		return TX_CONTINUE;

	if (tx->flags & IEEE80211_TX_PS_BUFFERED)
		return TX_CONTINUE;

	if (tx->sta)
		assoc = test_sta_flag(tx->sta, WLAN_STA_ASSOC);

	if (likely(tx->flags & IEEE80211_TX_UNICAST)) {
		if (unlikely(!assoc &&
			     ieee80211_is_data(hdr->frame_control))) {
#ifdef CONFIG_MAC80211_VERBOSE_DEBUG
			sdata_info(tx->sdata,
				   "dropped data frame to not associated station %pM\n",
				   hdr->addr1);
#endif
			I802_DEBUG_INC(tx->local->tx_handlers_drop_not_assoc);
			return TX_DROP;
		}
	} else if (unlikely(tx->sdata->vif.type == NL80211_IFTYPE_AP &&
			    ieee80211_is_data(hdr->frame_control) &&
			    !atomic_read(&tx->sdata->u.ap.num_mcast_sta))) {
		/*
		 * No associated STAs - no need to send multicast
		 * frames.
		 */
		return TX_DROP;
	}

	return TX_CONTINUE;
}
