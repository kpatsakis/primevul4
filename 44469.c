void ath_tx_node_cleanup(struct ath_softc *sc, struct ath_node *an)
{
	struct ath_atx_ac *ac;
	struct ath_atx_tid *tid;
	struct ath_txq *txq;
	int tidno;

	for (tidno = 0, tid = &an->tid[tidno];
	     tidno < IEEE80211_NUM_TIDS; tidno++, tid++) {

		ac = tid->ac;
		txq = ac->txq;

		ath_txq_lock(sc, txq);

		if (tid->sched) {
			list_del(&tid->list);
			tid->sched = false;
		}

		if (ac->sched) {
			list_del(&ac->list);
			tid->ac->sched = false;
		}

		ath_tid_drain(sc, txq, tid);
		tid->active = false;

		ath_txq_unlock(sc, txq);
	}
}
