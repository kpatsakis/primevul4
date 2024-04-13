void ath_tx_edma_tasklet(struct ath_softc *sc)
{
	struct ath_tx_status ts;
	struct ath_common *common = ath9k_hw_common(sc->sc_ah);
	struct ath_hw *ah = sc->sc_ah;
	struct ath_txq *txq;
	struct ath_buf *bf, *lastbf;
	struct list_head bf_head;
	struct list_head *fifo_list;
	int status;

	for (;;) {
		if (test_bit(SC_OP_HW_RESET, &sc->sc_flags))
			break;

		status = ath9k_hw_txprocdesc(ah, NULL, (void *)&ts);
		if (status == -EINPROGRESS)
			break;
		if (status == -EIO) {
			ath_dbg(common, XMIT, "Error processing tx status\n");
			break;
		}

		/* Process beacon completions separately */
		if (ts.qid == sc->beacon.beaconq) {
			sc->beacon.tx_processed = true;
			sc->beacon.tx_last = !(ts.ts_status & ATH9K_TXERR_MASK);

			ath9k_csa_is_finished(sc);
			continue;
		}

		txq = &sc->tx.txq[ts.qid];

		ath_txq_lock(sc, txq);

		TX_STAT_INC(txq->axq_qnum, txprocdesc);

		fifo_list = &txq->txq_fifo[txq->txq_tailidx];
		if (list_empty(fifo_list)) {
			ath_txq_unlock(sc, txq);
			return;
		}

		bf = list_first_entry(fifo_list, struct ath_buf, list);
		if (bf->bf_state.stale) {
			list_del(&bf->list);
			ath_tx_return_buffer(sc, bf);
			bf = list_first_entry(fifo_list, struct ath_buf, list);
		}

		lastbf = bf->bf_lastbf;

		INIT_LIST_HEAD(&bf_head);
		if (list_is_last(&lastbf->list, fifo_list)) {
			list_splice_tail_init(fifo_list, &bf_head);
			INCR(txq->txq_tailidx, ATH_TXFIFO_DEPTH);

			if (!list_empty(&txq->axq_q)) {
				struct list_head bf_q;

				INIT_LIST_HEAD(&bf_q);
				txq->axq_link = NULL;
				list_splice_tail_init(&txq->axq_q, &bf_q);
				ath_tx_txqaddbuf(sc, txq, &bf_q, true);
			}
		} else {
			lastbf->bf_state.stale = true;
			if (bf != lastbf)
				list_cut_position(&bf_head, fifo_list,
						  lastbf->list.prev);
		}

		ath_tx_process_buffer(sc, txq, &ts, bf, &bf_head);
		ath_txq_unlock_complete(sc, txq);
	}
}
