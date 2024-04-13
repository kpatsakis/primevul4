static bool ath_tx_sched_aggr(struct ath_softc *sc, struct ath_txq *txq,
			      struct ath_atx_tid *tid, bool *stop)
{
	struct ath_buf *bf;
	struct ieee80211_tx_info *tx_info;
	struct sk_buff_head *tid_q;
	struct list_head bf_q;
	int aggr_len = 0;
	bool aggr, last = true;

	if (!ath_tid_has_buffered(tid))
		return false;

	INIT_LIST_HEAD(&bf_q);

	bf = ath_tx_get_tid_subframe(sc, txq, tid, &tid_q);
	if (!bf)
		return false;

	tx_info = IEEE80211_SKB_CB(bf->bf_mpdu);
	aggr = !!(tx_info->flags & IEEE80211_TX_CTL_AMPDU);
	if ((aggr && txq->axq_ampdu_depth >= ATH_AGGR_MIN_QDEPTH) ||
		(!aggr && txq->axq_depth >= ATH_NON_AGGR_MIN_QDEPTH)) {
		*stop = true;
		return false;
	}

	ath_set_rates(tid->an->vif, tid->an->sta, bf);
	if (aggr)
		last = ath_tx_form_aggr(sc, txq, tid, &bf_q, bf,
					tid_q, &aggr_len);
	else
		ath_tx_form_burst(sc, txq, tid, &bf_q, bf, tid_q);

	if (list_empty(&bf_q))
		return false;

	if (tid->ac->clear_ps_filter || tid->an->no_ps_filter) {
		tid->ac->clear_ps_filter = false;
		tx_info->flags |= IEEE80211_TX_CTL_CLEAR_PS_FILT;
	}

	ath_tx_fill_desc(sc, bf, txq, aggr_len);
	ath_tx_txqaddbuf(sc, txq, &bf_q, false);
	return true;
}
