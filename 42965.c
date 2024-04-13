ieee80211_tx_h_rate_ctrl(struct ieee80211_tx_data *tx)
{
	struct ieee80211_tx_info *info = IEEE80211_SKB_CB(tx->skb);
	struct ieee80211_hdr *hdr = (void *)tx->skb->data;
	struct ieee80211_supported_band *sband;
	u32 len;
	struct ieee80211_tx_rate_control txrc;
	struct ieee80211_sta_rates *ratetbl = NULL;
	bool assoc = false;

	memset(&txrc, 0, sizeof(txrc));

	sband = tx->local->hw.wiphy->bands[info->band];

	len = min_t(u32, tx->skb->len + FCS_LEN,
			 tx->local->hw.wiphy->frag_threshold);

	/* set up the tx rate control struct we give the RC algo */
	txrc.hw = &tx->local->hw;
	txrc.sband = sband;
	txrc.bss_conf = &tx->sdata->vif.bss_conf;
	txrc.skb = tx->skb;
	txrc.reported_rate.idx = -1;
	txrc.rate_idx_mask = tx->sdata->rc_rateidx_mask[info->band];
	if (txrc.rate_idx_mask == (1 << sband->n_bitrates) - 1)
		txrc.max_rate_idx = -1;
	else
		txrc.max_rate_idx = fls(txrc.rate_idx_mask) - 1;

	if (tx->sdata->rc_has_mcs_mask[info->band])
		txrc.rate_idx_mcs_mask =
			tx->sdata->rc_rateidx_mcs_mask[info->band];

	txrc.bss = (tx->sdata->vif.type == NL80211_IFTYPE_AP ||
		    tx->sdata->vif.type == NL80211_IFTYPE_MESH_POINT ||
		    tx->sdata->vif.type == NL80211_IFTYPE_ADHOC);

	/* set up RTS protection if desired */
	if (len > tx->local->hw.wiphy->rts_threshold) {
		txrc.rts = true;
	}

	info->control.use_rts = txrc.rts;
	info->control.use_cts_prot = tx->sdata->vif.bss_conf.use_cts_prot;

	/*
	 * Use short preamble if the BSS can handle it, but not for
	 * management frames unless we know the receiver can handle
	 * that -- the management frame might be to a station that
	 * just wants a probe response.
	 */
	if (tx->sdata->vif.bss_conf.use_short_preamble &&
	    (ieee80211_is_data(hdr->frame_control) ||
	     (tx->sta && test_sta_flag(tx->sta, WLAN_STA_SHORT_PREAMBLE))))
		txrc.short_preamble = true;

	info->control.short_preamble = txrc.short_preamble;

	if (tx->sta)
		assoc = test_sta_flag(tx->sta, WLAN_STA_ASSOC);

	/*
	 * Lets not bother rate control if we're associated and cannot
	 * talk to the sta. This should not happen.
	 */
	if (WARN(test_bit(SCAN_SW_SCANNING, &tx->local->scanning) && assoc &&
		 !rate_usable_index_exists(sband, &tx->sta->sta),
		 "%s: Dropped data frame as no usable bitrate found while "
		 "scanning and associated. Target station: "
		 "%pM on %d GHz band\n",
		 tx->sdata->name, hdr->addr1,
		 info->band ? 5 : 2))
		return TX_DROP;

	/*
	 * If we're associated with the sta at this point we know we can at
	 * least send the frame at the lowest bit rate.
	 */
	rate_control_get_rate(tx->sdata, tx->sta, &txrc);

	if (tx->sta && !info->control.skip_table)
		ratetbl = rcu_dereference(tx->sta->sta.rates);

	if (unlikely(info->control.rates[0].idx < 0)) {
		if (ratetbl) {
			struct ieee80211_tx_rate rate = {
				.idx = ratetbl->rate[0].idx,
				.flags = ratetbl->rate[0].flags,
				.count = ratetbl->rate[0].count
			};

			if (ratetbl->rate[0].idx < 0)
				return TX_DROP;

			tx->rate = rate;
		} else {
			return TX_DROP;
		}
	} else {
		tx->rate = info->control.rates[0];
	}

	if (txrc.reported_rate.idx < 0) {
		txrc.reported_rate = tx->rate;
		if (tx->sta && ieee80211_is_data(hdr->frame_control))
			tx->sta->last_tx_rate = txrc.reported_rate;
	} else if (tx->sta)
		tx->sta->last_tx_rate = txrc.reported_rate;

	if (ratetbl)
		return TX_CONTINUE;

	if (unlikely(!info->control.rates[0].count))
		info->control.rates[0].count = 1;

	if (WARN_ON_ONCE((info->control.rates[0].count > 1) &&
			 (info->flags & IEEE80211_TX_CTL_NO_ACK)))
		info->control.rates[0].count = 1;

	return TX_CONTINUE;
}
