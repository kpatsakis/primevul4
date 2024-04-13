ieee80211_tx_h_check_control_port_protocol(struct ieee80211_tx_data *tx)
{
	struct ieee80211_tx_info *info = IEEE80211_SKB_CB(tx->skb);

	if (unlikely(tx->sdata->control_port_protocol == tx->skb->protocol)) {
		if (tx->sdata->control_port_no_encrypt)
			info->flags |= IEEE80211_TX_INTFL_DONT_ENCRYPT;
		info->control.flags |= IEEE80211_TX_CTRL_PORT_CTRL_PROTO;
	}

	return TX_CONTINUE;
}
