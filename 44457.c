void ath_tx_cabq(struct ieee80211_hw *hw, struct ieee80211_vif *vif,
		 struct sk_buff *skb)
{
	struct ath_softc *sc = hw->priv;
	struct ath_tx_control txctl = {
		.txq = sc->beacon.cabq
	};
	struct ath_tx_info info = {};
	struct ieee80211_hdr *hdr;
	struct ath_buf *bf_tail = NULL;
	struct ath_buf *bf;
	LIST_HEAD(bf_q);
	int duration = 0;
	int max_duration;

	max_duration =
		sc->cur_beacon_conf.beacon_interval * 1000 *
		sc->cur_beacon_conf.dtim_period / ATH_BCBUF;

	do {
		struct ath_frame_info *fi = get_frame_info(skb);

		if (ath_tx_prepare(hw, skb, &txctl))
			break;

		bf = ath_tx_setup_buffer(sc, txctl.txq, NULL, skb);
		if (!bf)
			break;

		bf->bf_lastbf = bf;
		ath_set_rates(vif, NULL, bf);
		ath_buf_set_rate(sc, bf, &info, fi->framelen, false);
		duration += info.rates[0].PktDuration;
		if (bf_tail)
			bf_tail->bf_next = bf;

		list_add_tail(&bf->list, &bf_q);
		bf_tail = bf;
		skb = NULL;

		if (duration > max_duration)
			break;

		skb = ieee80211_get_buffered_bc(hw, vif);
	} while(skb);

	if (skb)
		ieee80211_free_txskb(hw, skb);

	if (list_empty(&bf_q))
		return;

	bf = list_first_entry(&bf_q, struct ath_buf, list);
	hdr = (struct ieee80211_hdr *) bf->bf_mpdu->data;

	if (hdr->frame_control & IEEE80211_FCTL_MOREDATA) {
		hdr->frame_control &= ~IEEE80211_FCTL_MOREDATA;
		dma_sync_single_for_device(sc->dev, bf->bf_buf_addr,
			sizeof(*hdr), DMA_TO_DEVICE);
	}

	ath_txq_lock(sc, txctl.txq);
	ath_tx_fill_desc(sc, bf, txctl.txq, 0);
	ath_tx_txqaddbuf(sc, txctl.txq, &bf_q, false);
	TX_STAT_INC(txctl.txq->axq_qnum, queued);
	ath_txq_unlock(sc, txctl.txq);
}
