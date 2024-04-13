static void ath_tx_process_buffer(struct ath_softc *sc, struct ath_txq *txq,
				  struct ath_tx_status *ts, struct ath_buf *bf,
				  struct list_head *bf_head)
{
	struct ieee80211_tx_info *info;
	bool txok, flush;

	txok = !(ts->ts_status & ATH9K_TXERR_MASK);
	flush = !!(ts->ts_status & ATH9K_TX_FLUSH);
	txq->axq_tx_inprogress = false;

	txq->axq_depth--;
	if (bf_is_ampdu_not_probing(bf))
		txq->axq_ampdu_depth--;

	if (!bf_isampdu(bf)) {
		if (!flush) {
			info = IEEE80211_SKB_CB(bf->bf_mpdu);
			memcpy(info->control.rates, bf->rates,
			       sizeof(info->control.rates));
			ath_tx_rc_status(sc, bf, ts, 1, txok ? 0 : 1, txok);
		}
		ath_tx_complete_buf(sc, bf, txq, bf_head, ts, txok);
	} else
		ath_tx_complete_aggr(sc, txq, bf, bf_head, ts, txok);

	if (!flush)
		ath_txq_schedule(sc, txq);
}
