static void ath_tx_processq(struct ath_softc *sc, struct ath_txq *txq)
{
	struct ath_hw *ah = sc->sc_ah;
	struct ath_common *common = ath9k_hw_common(ah);
	struct ath_buf *bf, *lastbf, *bf_held = NULL;
	struct list_head bf_head;
	struct ath_desc *ds;
	struct ath_tx_status ts;
	int status;

	ath_dbg(common, QUEUE, "tx queue %d (%x), link %p\n",
		txq->axq_qnum, ath9k_hw_gettxbuf(sc->sc_ah, txq->axq_qnum),
		txq->axq_link);

	ath_txq_lock(sc, txq);
	for (;;) {
		if (test_bit(SC_OP_HW_RESET, &sc->sc_flags))
			break;

		if (list_empty(&txq->axq_q)) {
			txq->axq_link = NULL;
			ath_txq_schedule(sc, txq);
			break;
		}
		bf = list_first_entry(&txq->axq_q, struct ath_buf, list);

		/*
		 * There is a race condition that a BH gets scheduled
		 * after sw writes TxE and before hw re-load the last
		 * descriptor to get the newly chained one.
		 * Software must keep the last DONE descriptor as a
		 * holding descriptor - software does so by marking
		 * it with the STALE flag.
		 */
		bf_held = NULL;
		if (bf->bf_state.stale) {
			bf_held = bf;
			if (list_is_last(&bf_held->list, &txq->axq_q))
				break;

			bf = list_entry(bf_held->list.next, struct ath_buf,
					list);
		}

		lastbf = bf->bf_lastbf;
		ds = lastbf->bf_desc;

		memset(&ts, 0, sizeof(ts));
		status = ath9k_hw_txprocdesc(ah, ds, &ts);
		if (status == -EINPROGRESS)
			break;

		TX_STAT_INC(txq->axq_qnum, txprocdesc);

		/*
		 * Remove ath_buf's of the same transmit unit from txq,
		 * however leave the last descriptor back as the holding
		 * descriptor for hw.
		 */
		lastbf->bf_state.stale = true;
		INIT_LIST_HEAD(&bf_head);
		if (!list_is_singular(&lastbf->list))
			list_cut_position(&bf_head,
				&txq->axq_q, lastbf->list.prev);

		if (bf_held) {
			list_del(&bf_held->list);
			ath_tx_return_buffer(sc, bf_held);
		}

		ath_tx_process_buffer(sc, txq, &ts, bf, &bf_head);
	}
	ath_txq_unlock_complete(sc, txq);
}
