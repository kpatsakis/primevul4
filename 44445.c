static u32 ath_lookup_rate(struct ath_softc *sc, struct ath_buf *bf,
			   struct ath_atx_tid *tid)
{
	struct sk_buff *skb;
	struct ieee80211_tx_info *tx_info;
	struct ieee80211_tx_rate *rates;
	u32 max_4ms_framelen, frmlen;
	u16 aggr_limit, bt_aggr_limit, legacy = 0;
	int q = tid->ac->txq->mac80211_qnum;
	int i;

	skb = bf->bf_mpdu;
	tx_info = IEEE80211_SKB_CB(skb);
	rates = bf->rates;

	/*
	 * Find the lowest frame length among the rate series that will have a
	 * 4ms (or TXOP limited) transmit duration.
	 */
	max_4ms_framelen = ATH_AMPDU_LIMIT_MAX;

	for (i = 0; i < 4; i++) {
		int modeidx;

		if (!rates[i].count)
			continue;

		if (!(rates[i].flags & IEEE80211_TX_RC_MCS)) {
			legacy = 1;
			break;
		}

		if (rates[i].flags & IEEE80211_TX_RC_40_MHZ_WIDTH)
			modeidx = MCS_HT40;
		else
			modeidx = MCS_HT20;

		if (rates[i].flags & IEEE80211_TX_RC_SHORT_GI)
			modeidx++;

		frmlen = sc->tx.max_aggr_framelen[q][modeidx][rates[i].idx];
		max_4ms_framelen = min(max_4ms_framelen, frmlen);
	}

	/*
	 * limit aggregate size by the minimum rate if rate selected is
	 * not a probe rate, if rate selected is a probe rate then
	 * avoid aggregation of this packet.
	 */
	if (tx_info->flags & IEEE80211_TX_CTL_RATE_CTRL_PROBE || legacy)
		return 0;

	aggr_limit = min(max_4ms_framelen, (u32)ATH_AMPDU_LIMIT_MAX);

	/*
	 * Override the default aggregation limit for BTCOEX.
	 */
	bt_aggr_limit = ath9k_btcoex_aggr_limit(sc, max_4ms_framelen);
	if (bt_aggr_limit)
		aggr_limit = bt_aggr_limit;

	if (tid->an->maxampdu)
		aggr_limit = min(aggr_limit, tid->an->maxampdu);

	return aggr_limit;
}
