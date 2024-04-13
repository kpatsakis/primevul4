int ath_tx_aggr_start(struct ath_softc *sc, struct ieee80211_sta *sta,
		      u16 tid, u16 *ssn)
{
	struct ath_atx_tid *txtid;
	struct ath_txq *txq;
	struct ath_node *an;
	u8 density;

	an = (struct ath_node *)sta->drv_priv;
	txtid = ATH_AN_2_TID(an, tid);
	txq = txtid->ac->txq;

	ath_txq_lock(sc, txq);

	/* update ampdu factor/density, they may have changed. This may happen
	 * in HT IBSS when a beacon with HT-info is received after the station
	 * has already been added.
	 */
	if (sta->ht_cap.ht_supported) {
		an->maxampdu = (1 << (IEEE80211_HT_MAX_AMPDU_FACTOR +
				      sta->ht_cap.ampdu_factor)) - 1;
		density = ath9k_parse_mpdudensity(sta->ht_cap.ampdu_density);
		an->mpdudensity = density;
	}

	/* force sequence number allocation for pending frames */
	ath_tx_tid_change_state(sc, txtid);

	txtid->active = true;
	txtid->paused = true;
	*ssn = txtid->seq_start = txtid->seq_next;
	txtid->bar_index = -1;

	memset(txtid->tx_buf, 0, sizeof(txtid->tx_buf));
	txtid->baw_head = txtid->baw_tail = 0;

	ath_txq_unlock_complete(sc, txq);

	return 0;
}
