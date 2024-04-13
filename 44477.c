static void ath_tx_send_normal(struct ath_softc *sc, struct ath_txq *txq,
			       struct ath_atx_tid *tid, struct sk_buff *skb)
{
	struct ieee80211_tx_info *tx_info = IEEE80211_SKB_CB(skb);
	struct ath_frame_info *fi = get_frame_info(skb);
	struct list_head bf_head;
	struct ath_buf *bf = fi->bf;

	INIT_LIST_HEAD(&bf_head);
	list_add_tail(&bf->list, &bf_head);
	bf->bf_state.bf_type = 0;
	if (tid && (tx_info->flags & IEEE80211_TX_CTL_AMPDU)) {
		bf->bf_state.bf_type = BUF_AMPDU;
		ath_tx_addto_baw(sc, tid, bf);
	}

	bf->bf_next = NULL;
	bf->bf_lastbf = bf;
	ath_tx_fill_desc(sc, bf, txq, fi->framelen);
	ath_tx_txqaddbuf(sc, txq, &bf_head, false);
	TX_STAT_INC(txq->axq_qnum, queued);
}
