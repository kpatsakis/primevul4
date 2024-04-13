static void ath_tx_complete_buf(struct ath_softc *sc, struct ath_buf *bf,
				struct ath_txq *txq, struct list_head *bf_q,
				struct ath_tx_status *ts, int txok)
{
	struct sk_buff *skb = bf->bf_mpdu;
	struct ieee80211_tx_info *tx_info = IEEE80211_SKB_CB(skb);
	unsigned long flags;
	int tx_flags = 0;

	if (!txok)
		tx_flags |= ATH_TX_ERROR;

	if (ts->ts_status & ATH9K_TXERR_FILT)
		tx_info->flags |= IEEE80211_TX_STAT_TX_FILTERED;

	dma_unmap_single(sc->dev, bf->bf_buf_addr, skb->len, DMA_TO_DEVICE);
	bf->bf_buf_addr = 0;
	if (sc->tx99_state)
		goto skip_tx_complete;

	if (bf->bf_state.bfs_paprd) {
		if (time_after(jiffies,
				bf->bf_state.bfs_paprd_timestamp +
				msecs_to_jiffies(ATH_PAPRD_TIMEOUT)))
			dev_kfree_skb_any(skb);
		else
			complete(&sc->paprd_complete);
	} else {
		ath_debug_stat_tx(sc, bf, ts, txq, tx_flags);
		ath_tx_complete(sc, skb, tx_flags, txq);
	}
skip_tx_complete:
	/* At this point, skb (bf->bf_mpdu) is consumed...make sure we don't
	 * accidentally reference it later.
	 */
	bf->bf_mpdu = NULL;

	/*
	 * Return the list of ath_buf of this mpdu to free queue
	 */
	spin_lock_irqsave(&sc->tx.txbuflock, flags);
	list_splice_tail_init(bf_q, &sc->tx.txbuf);
	spin_unlock_irqrestore(&sc->tx.txbuflock, flags);
}
