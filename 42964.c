ieee80211_tx_h_ps_buf(struct ieee80211_tx_data *tx)
{
	struct ieee80211_tx_info *info = IEEE80211_SKB_CB(tx->skb);
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *)tx->skb->data;

	if (unlikely(tx->flags & IEEE80211_TX_PS_BUFFERED))
		return TX_CONTINUE;

	/* only deauth, disassoc and action are bufferable MMPDUs */
	if (ieee80211_is_mgmt(hdr->frame_control) &&
	    !ieee80211_is_deauth(hdr->frame_control) &&
	    !ieee80211_is_disassoc(hdr->frame_control) &&
	    !ieee80211_is_action(hdr->frame_control)) {
		if (tx->flags & IEEE80211_TX_UNICAST)
			info->flags |= IEEE80211_TX_CTL_NO_PS_BUFFER;
		return TX_CONTINUE;
	}

	if (tx->flags & IEEE80211_TX_UNICAST)
		return ieee80211_tx_h_unicast_ps_buf(tx);
	else
		return ieee80211_tx_h_multicast_ps_buf(tx);
}
