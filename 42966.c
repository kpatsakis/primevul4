ieee80211_tx_h_select_key(struct ieee80211_tx_data *tx)
{
	struct ieee80211_key *key;
	struct ieee80211_tx_info *info = IEEE80211_SKB_CB(tx->skb);
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *)tx->skb->data;

	if (unlikely(info->flags & IEEE80211_TX_INTFL_DONT_ENCRYPT))
		tx->key = NULL;
	else if (tx->sta &&
		 (key = rcu_dereference(tx->sta->ptk[tx->sta->ptk_idx])))
		tx->key = key;
	else if (ieee80211_is_mgmt(hdr->frame_control) &&
		 is_multicast_ether_addr(hdr->addr1) &&
		 ieee80211_is_robust_mgmt_frame(hdr) &&
		 (key = rcu_dereference(tx->sdata->default_mgmt_key)))
		tx->key = key;
	else if (is_multicast_ether_addr(hdr->addr1) &&
		 (key = rcu_dereference(tx->sdata->default_multicast_key)))
		tx->key = key;
	else if (!is_multicast_ether_addr(hdr->addr1) &&
		 (key = rcu_dereference(tx->sdata->default_unicast_key)))
		tx->key = key;
	else if (info->flags & IEEE80211_TX_CTL_INJECTED)
		tx->key = NULL;
	else if (!tx->sdata->drop_unencrypted)
		tx->key = NULL;
	else if (tx->skb->protocol == tx->sdata->control_port_protocol)
		tx->key = NULL;
	else if (ieee80211_is_robust_mgmt_frame(hdr) &&
		 !(ieee80211_is_action(hdr->frame_control) &&
		   tx->sta && test_sta_flag(tx->sta, WLAN_STA_MFP)))
		tx->key = NULL;
	else if (ieee80211_is_mgmt(hdr->frame_control) &&
		 !ieee80211_is_robust_mgmt_frame(hdr))
		tx->key = NULL;
	else {
		I802_DEBUG_INC(tx->local->tx_handlers_drop_unencrypted);
		return TX_DROP;
	}

	if (tx->key) {
		bool skip_hw = false;

		tx->key->tx_rx_count++;
		/* TODO: add threshold stuff again */

		switch (tx->key->conf.cipher) {
		case WLAN_CIPHER_SUITE_WEP40:
		case WLAN_CIPHER_SUITE_WEP104:
		case WLAN_CIPHER_SUITE_TKIP:
			if (!ieee80211_is_data_present(hdr->frame_control))
				tx->key = NULL;
			break;
		case WLAN_CIPHER_SUITE_CCMP:
			if (!ieee80211_is_data_present(hdr->frame_control) &&
			    !ieee80211_use_mfp(hdr->frame_control, tx->sta,
					       tx->skb))
				tx->key = NULL;
			else
				skip_hw = (tx->key->conf.flags &
					   IEEE80211_KEY_FLAG_SW_MGMT_TX) &&
					ieee80211_is_mgmt(hdr->frame_control);
			break;
		case WLAN_CIPHER_SUITE_AES_CMAC:
			if (!ieee80211_is_mgmt(hdr->frame_control))
				tx->key = NULL;
			break;
		}

		if (unlikely(tx->key && tx->key->flags & KEY_FLAG_TAINTED &&
			     !ieee80211_is_deauth(hdr->frame_control)))
			return TX_DROP;

		if (!skip_hw && tx->key &&
		    tx->key->flags & KEY_FLAG_UPLOADED_TO_HARDWARE)
			info->control.hw_key = &tx->key->conf;
	}

	return TX_CONTINUE;
}
