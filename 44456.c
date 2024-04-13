void ath_tx_aggr_wakeup(struct ath_softc *sc, struct ath_node *an)
{
	struct ath_atx_tid *tid;
	struct ath_atx_ac *ac;
	struct ath_txq *txq;
	int tidno;

	for (tidno = 0, tid = &an->tid[tidno];
	     tidno < IEEE80211_NUM_TIDS; tidno++, tid++) {

		ac = tid->ac;
		txq = ac->txq;

		ath_txq_lock(sc, txq);
		ac->clear_ps_filter = true;

		if (!tid->paused && ath_tid_has_buffered(tid)) {
			ath_tx_queue_tid(txq, tid);
			ath_txq_schedule(sc, txq);
		}

		ath_txq_unlock_complete(sc, txq);
	}
}
