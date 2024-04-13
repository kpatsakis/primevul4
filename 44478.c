static void ath_tx_set_retry(struct ath_softc *sc, struct ath_txq *txq,
			     struct sk_buff *skb, int count)
{
	struct ath_frame_info *fi = get_frame_info(skb);
	struct ath_buf *bf = fi->bf;
	struct ieee80211_hdr *hdr;
	int prev = fi->retries;

	TX_STAT_INC(txq->axq_qnum, a_retries);
	fi->retries += count;

	if (prev > 0)
		return;

	hdr = (struct ieee80211_hdr *)skb->data;
	hdr->frame_control |= cpu_to_le16(IEEE80211_FCTL_RETRY);
	dma_sync_single_for_device(sc->dev, bf->bf_buf_addr,
		sizeof(*hdr), DMA_TO_DEVICE);
}
