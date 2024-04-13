ath_tx_form_burst(struct ath_softc *sc, struct ath_txq *txq,
		  struct ath_atx_tid *tid, struct list_head *bf_q,
		  struct ath_buf *bf_first, struct sk_buff_head *tid_q)
{
	struct ath_buf *bf = bf_first, *bf_prev = NULL;
	struct sk_buff *skb;
	int nframes = 0;

	do {
		struct ieee80211_tx_info *tx_info;
		skb = bf->bf_mpdu;

		nframes++;
		__skb_unlink(skb, tid_q);
		list_add_tail(&bf->list, bf_q);
		if (bf_prev)
			bf_prev->bf_next = bf;
		bf_prev = bf;

		if (nframes >= 2)
			break;

		bf = ath_tx_get_tid_subframe(sc, txq, tid, &tid_q);
		if (!bf)
			break;

		tx_info = IEEE80211_SKB_CB(bf->bf_mpdu);
		if (tx_info->flags & IEEE80211_TX_CTL_AMPDU)
			break;

		ath_set_rates(tid->an->vif, tid->an->sta, bf);
	} while (1);
}
