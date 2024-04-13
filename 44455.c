void ath_tx_aggr_stop(struct ath_softc *sc, struct ieee80211_sta *sta, u16 tid)
{
	struct ath_node *an = (struct ath_node *)sta->drv_priv;
	struct ath_atx_tid *txtid = ATH_AN_2_TID(an, tid);
	struct ath_txq *txq = txtid->ac->txq;

	ath_txq_lock(sc, txq);
	txtid->active = false;
	txtid->paused = false;
	ath_tx_flush_tid(sc, txtid);
	ath_tx_tid_change_state(sc, txtid);
	ath_txq_unlock_complete(sc, txq);
}
