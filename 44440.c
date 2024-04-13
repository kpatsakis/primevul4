bool ath_drain_all_txq(struct ath_softc *sc)
{
	struct ath_hw *ah = sc->sc_ah;
	struct ath_common *common = ath9k_hw_common(sc->sc_ah);
	struct ath_txq *txq;
	int i;
	u32 npend = 0;

	if (test_bit(SC_OP_INVALID, &sc->sc_flags))
		return true;

	ath9k_hw_abort_tx_dma(ah);

	/* Check if any queue remains active */
	for (i = 0; i < ATH9K_NUM_TX_QUEUES; i++) {
		if (!ATH_TXQ_SETUP(sc, i))
			continue;

		if (!sc->tx.txq[i].axq_depth)
			continue;

		if (ath9k_hw_numtxpending(ah, sc->tx.txq[i].axq_qnum))
			npend |= BIT(i);
	}

	if (npend)
		ath_err(common, "Failed to stop TX DMA, queues=0x%03x!\n", npend);

	for (i = 0; i < ATH9K_NUM_TX_QUEUES; i++) {
		if (!ATH_TXQ_SETUP(sc, i))
			continue;

		/*
		 * The caller will resume queues with ieee80211_wake_queues.
		 * Mark the queue as not stopped to prevent ath_tx_complete
		 * from waking the queue too early.
		 */
		txq = &sc->tx.txq[i];
		txq->stopped = false;
		ath_draintxq(sc, txq);
	}

	return !npend;
}
