static void ath_drain_txq_list(struct ath_softc *sc, struct ath_txq *txq,
			       struct list_head *list)
{
	struct ath_buf *bf, *lastbf;
	struct list_head bf_head;
	struct ath_tx_status ts;

	memset(&ts, 0, sizeof(ts));
	ts.ts_status = ATH9K_TX_FLUSH;
	INIT_LIST_HEAD(&bf_head);

	while (!list_empty(list)) {
		bf = list_first_entry(list, struct ath_buf, list);

		if (bf->bf_state.stale) {
			list_del(&bf->list);

			ath_tx_return_buffer(sc, bf);
			continue;
		}

		lastbf = bf->bf_lastbf;
		list_cut_position(&bf_head, list, &lastbf->list);
		ath_tx_process_buffer(sc, txq, &ts, bf, &bf_head);
	}
}
