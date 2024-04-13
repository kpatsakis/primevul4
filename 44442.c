void ath_draintxq(struct ath_softc *sc, struct ath_txq *txq)
{
	ath_txq_lock(sc, txq);

	if (sc->sc_ah->caps.hw_caps & ATH9K_HW_CAP_EDMA) {
		int idx = txq->txq_tailidx;

		while (!list_empty(&txq->txq_fifo[idx])) {
			ath_drain_txq_list(sc, txq, &txq->txq_fifo[idx]);

			INCR(idx, ATH_TXFIFO_DEPTH);
		}
		txq->txq_tailidx = idx;
	}

	txq->axq_link = NULL;
	txq->axq_tx_inprogress = false;
	ath_drain_txq_list(sc, txq, &txq->axq_q);

	ath_txq_unlock_complete(sc, txq);
}
