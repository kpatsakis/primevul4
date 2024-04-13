struct sk_buff *ieee80211_beacon_get_tim(struct ieee80211_hw *hw,
					 struct ieee80211_vif *vif,
					 u16 *tim_offset, u16 *tim_length)
{
	struct ieee80211_local *local = hw_to_local(hw);
	struct sk_buff *skb = NULL;
	struct ieee80211_tx_info *info;
	struct ieee80211_sub_if_data *sdata = NULL;
	enum ieee80211_band band;
	struct ieee80211_tx_rate_control txrc;
	struct ieee80211_chanctx_conf *chanctx_conf;

	rcu_read_lock();

	sdata = vif_to_sdata(vif);
	chanctx_conf = rcu_dereference(sdata->vif.chanctx_conf);

	if (!ieee80211_sdata_running(sdata) || !chanctx_conf)
		goto out;

	if (tim_offset)
		*tim_offset = 0;
	if (tim_length)
		*tim_length = 0;

	if (sdata->vif.type == NL80211_IFTYPE_AP) {
		struct ieee80211_if_ap *ap = &sdata->u.ap;
		struct beacon_data *beacon = rcu_dereference(ap->beacon);

		if (beacon) {
			if (sdata->vif.csa_active)
				ieee80211_update_csa(sdata, beacon);

			/*
			 * headroom, head length,
			 * tail length and maximum TIM length
			 */
			skb = dev_alloc_skb(local->tx_headroom +
					    beacon->head_len +
					    beacon->tail_len + 256 +
					    local->hw.extra_beacon_tailroom);
			if (!skb)
				goto out;

			skb_reserve(skb, local->tx_headroom);
			memcpy(skb_put(skb, beacon->head_len), beacon->head,
			       beacon->head_len);

			ieee80211_beacon_add_tim(sdata, &ap->ps, skb);

			if (tim_offset)
				*tim_offset = beacon->head_len;
			if (tim_length)
				*tim_length = skb->len - beacon->head_len;

			if (beacon->tail)
				memcpy(skb_put(skb, beacon->tail_len),
				       beacon->tail, beacon->tail_len);
		} else
			goto out;
	} else if (sdata->vif.type == NL80211_IFTYPE_ADHOC) {
		struct ieee80211_if_ibss *ifibss = &sdata->u.ibss;
		struct ieee80211_hdr *hdr;
		struct beacon_data *presp = rcu_dereference(ifibss->presp);

		if (!presp)
			goto out;

		if (sdata->vif.csa_active)
			ieee80211_update_csa(sdata, presp);


		skb = dev_alloc_skb(local->tx_headroom + presp->head_len +
				    local->hw.extra_beacon_tailroom);
		if (!skb)
			goto out;
		skb_reserve(skb, local->tx_headroom);
		memcpy(skb_put(skb, presp->head_len), presp->head,
		       presp->head_len);

		hdr = (struct ieee80211_hdr *) skb->data;
		hdr->frame_control = cpu_to_le16(IEEE80211_FTYPE_MGMT |
						 IEEE80211_STYPE_BEACON);
	} else if (ieee80211_vif_is_mesh(&sdata->vif)) {
		struct ieee80211_if_mesh *ifmsh = &sdata->u.mesh;
		struct beacon_data *bcn = rcu_dereference(ifmsh->beacon);

		if (!bcn)
			goto out;

		if (sdata->vif.csa_active)
			ieee80211_update_csa(sdata, bcn);

		if (ifmsh->sync_ops)
			ifmsh->sync_ops->adjust_tbtt(sdata, bcn);

		skb = dev_alloc_skb(local->tx_headroom +
				    bcn->head_len +
				    256 + /* TIM IE */
				    bcn->tail_len +
				    local->hw.extra_beacon_tailroom);
		if (!skb)
			goto out;
		skb_reserve(skb, local->tx_headroom);
		memcpy(skb_put(skb, bcn->head_len), bcn->head, bcn->head_len);
		ieee80211_beacon_add_tim(sdata, &ifmsh->ps, skb);
		memcpy(skb_put(skb, bcn->tail_len), bcn->tail, bcn->tail_len);
	} else {
		WARN_ON(1);
		goto out;
	}

	band = chanctx_conf->def.chan->band;

	info = IEEE80211_SKB_CB(skb);

	info->flags |= IEEE80211_TX_INTFL_DONT_ENCRYPT;
	info->flags |= IEEE80211_TX_CTL_NO_ACK;
	info->band = band;

	memset(&txrc, 0, sizeof(txrc));
	txrc.hw = hw;
	txrc.sband = local->hw.wiphy->bands[band];
	txrc.bss_conf = &sdata->vif.bss_conf;
	txrc.skb = skb;
	txrc.reported_rate.idx = -1;
	txrc.rate_idx_mask = sdata->rc_rateidx_mask[band];
	if (txrc.rate_idx_mask == (1 << txrc.sband->n_bitrates) - 1)
		txrc.max_rate_idx = -1;
	else
		txrc.max_rate_idx = fls(txrc.rate_idx_mask) - 1;
	txrc.bss = true;
	rate_control_get_rate(sdata, NULL, &txrc);

	info->control.vif = vif;

	info->flags |= IEEE80211_TX_CTL_CLEAR_PS_FILT |
			IEEE80211_TX_CTL_ASSIGN_SEQ |
			IEEE80211_TX_CTL_FIRST_FRAGMENT;
 out:
	rcu_read_unlock();
	return skb;
}
