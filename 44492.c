static void setup_frame_info(struct ieee80211_hw *hw,
			     struct ieee80211_sta *sta,
			     struct sk_buff *skb,
			     int framelen)
{
	struct ieee80211_tx_info *tx_info = IEEE80211_SKB_CB(skb);
	struct ieee80211_key_conf *hw_key = tx_info->control.hw_key;
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *)skb->data;
	const struct ieee80211_rate *rate;
	struct ath_frame_info *fi = get_frame_info(skb);
	struct ath_node *an = NULL;
	enum ath9k_key_type keytype;
	bool short_preamble = false;

	/*
	 * We check if Short Preamble is needed for the CTS rate by
	 * checking the BSS's global flag.
	 * But for the rate series, IEEE80211_TX_RC_USE_SHORT_PREAMBLE is used.
	 */
	if (tx_info->control.vif &&
	    tx_info->control.vif->bss_conf.use_short_preamble)
		short_preamble = true;

	rate = ieee80211_get_rts_cts_rate(hw, tx_info);
	keytype = ath9k_cmn_get_hw_crypto_keytype(skb);

	if (sta)
		an = (struct ath_node *) sta->drv_priv;

	memset(fi, 0, sizeof(*fi));
	if (hw_key)
		fi->keyix = hw_key->hw_key_idx;
	else if (an && ieee80211_is_data(hdr->frame_control) && an->ps_key > 0)
		fi->keyix = an->ps_key;
	else
		fi->keyix = ATH9K_TXKEYIX_INVALID;
	fi->keytype = keytype;
	fi->framelen = framelen;

	if (!rate)
		return;
	fi->rtscts_rate = rate->hw_value;
	if (short_preamble)
		fi->rtscts_rate |= rate->hw_value_short;
}
