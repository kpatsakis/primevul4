int ath_tx_start(struct ieee80211_hw *hw, struct sk_buff *skb,
		 struct ath_tx_control *txctl)
{
	struct ieee80211_hdr *hdr;
	struct ieee80211_tx_info *info = IEEE80211_SKB_CB(skb);
	struct ieee80211_sta *sta = txctl->sta;
	struct ieee80211_vif *vif = info->control.vif;
	struct ath_softc *sc = hw->priv;
	struct ath_txq *txq = txctl->txq;
	struct ath_atx_tid *tid = NULL;
	struct ath_buf *bf;
	int q;
	int ret;

	ret = ath_tx_prepare(hw, skb, txctl);
	if (ret)
	    return ret;

	hdr = (struct ieee80211_hdr *) skb->data;
	/*
	 * At this point, the vif, hw_key and sta pointers in the tx control
	 * info are no longer valid (overwritten by the ath_frame_info data.
	 */

	q = skb_get_queue_mapping(skb);

	ath_txq_lock(sc, txq);
	if (txq == sc->tx.txq_map[q] &&
	    ++txq->pending_frames > sc->tx.txq_max_pending[q] &&
	    !txq->stopped) {
		ieee80211_stop_queue(sc->hw, q);
		txq->stopped = true;
	}

	if (info->flags & IEEE80211_TX_CTL_PS_RESPONSE) {
		ath_txq_unlock(sc, txq);
		txq = sc->tx.uapsdq;
		ath_txq_lock(sc, txq);
	} else if (txctl->an &&
		   ieee80211_is_data_present(hdr->frame_control)) {
		tid = ath_get_skb_tid(sc, txctl->an, skb);

		WARN_ON(tid->ac->txq != txctl->txq);

		if (info->flags & IEEE80211_TX_CTL_CLEAR_PS_FILT)
			tid->ac->clear_ps_filter = true;

		/*
		 * Add this frame to software queue for scheduling later
		 * for aggregation.
		 */
		TX_STAT_INC(txq->axq_qnum, a_queued_sw);
		__skb_queue_tail(&tid->buf_q, skb);
		if (!txctl->an->sleeping)
			ath_tx_queue_tid(txq, tid);

		ath_txq_schedule(sc, txq);
		goto out;
	}

	bf = ath_tx_setup_buffer(sc, txq, tid, skb);
	if (!bf) {
		ath_txq_skb_done(sc, txq, skb);
		if (txctl->paprd)
			dev_kfree_skb_any(skb);
		else
			ieee80211_free_txskb(sc->hw, skb);
		goto out;
	}

	bf->bf_state.bfs_paprd = txctl->paprd;

	if (txctl->paprd)
		bf->bf_state.bfs_paprd_timestamp = jiffies;

	ath_set_rates(vif, sta, bf);
	ath_tx_send_normal(sc, txq, tid, skb);

out:
	ath_txq_unlock(sc, txq);

	return 0;
}
