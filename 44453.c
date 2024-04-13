void ath_tx_aggr_resume(struct ath_softc *sc, struct ieee80211_sta *sta,
			u16 tidno)
{
	struct ath_atx_tid *tid;
	struct ath_node *an;
	struct ath_txq *txq;

	an = (struct ath_node *)sta->drv_priv;
	tid = ATH_AN_2_TID(an, tidno);
	txq = tid->ac->txq;

	ath_txq_lock(sc, txq);

	tid->baw_size = IEEE80211_MIN_AMPDU_BUF << sta->ht_cap.ampdu_factor;
	tid->paused = false;

	if (ath_tid_has_buffered(tid)) {
		ath_tx_queue_tid(txq, tid);
		ath_txq_schedule(sc, txq);
	}

	ath_txq_unlock_complete(sc, txq);
}
