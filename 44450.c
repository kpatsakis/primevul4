static void ath_tid_drain(struct ath_softc *sc, struct ath_txq *txq,
			  struct ath_atx_tid *tid)

{
	struct sk_buff *skb;
	struct ath_buf *bf;
	struct list_head bf_head;
	struct ath_tx_status ts;
	struct ath_frame_info *fi;

	memset(&ts, 0, sizeof(ts));
	INIT_LIST_HEAD(&bf_head);

	while ((skb = ath_tid_dequeue(tid))) {
		fi = get_frame_info(skb);
		bf = fi->bf;

		if (!bf) {
			ath_tx_complete(sc, skb, ATH_TX_ERROR, txq);
			continue;
		}

		list_add_tail(&bf->list, &bf_head);
		ath_tx_complete_buf(sc, bf, txq, &bf_head, &ts, 0);
	}
}
