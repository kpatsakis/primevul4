static void ath_tx_complete(struct ath_softc *sc, struct sk_buff *skb,
			    int tx_flags, struct ath_txq *txq)
{
	struct ieee80211_tx_info *tx_info = IEEE80211_SKB_CB(skb);
	struct ath_common *common = ath9k_hw_common(sc->sc_ah);
	struct ieee80211_hdr * hdr = (struct ieee80211_hdr *)skb->data;
	int padpos, padsize;
	unsigned long flags;

	ath_dbg(common, XMIT, "TX complete: skb: %p\n", skb);

	if (sc->sc_ah->caldata)
		set_bit(PAPRD_PACKET_SENT, &sc->sc_ah->caldata->cal_flags);

	if (!(tx_flags & ATH_TX_ERROR))
		/* Frame was ACKed */
		tx_info->flags |= IEEE80211_TX_STAT_ACK;

	padpos = ieee80211_hdrlen(hdr->frame_control);
	padsize = padpos & 3;
	if (padsize && skb->len>padpos+padsize) {
		/*
		 * Remove MAC header padding before giving the frame back to
		 * mac80211.
		 */
		memmove(skb->data + padsize, skb->data, padpos);
		skb_pull(skb, padsize);
	}

	spin_lock_irqsave(&sc->sc_pm_lock, flags);
	if ((sc->ps_flags & PS_WAIT_FOR_TX_ACK) && !txq->axq_depth) {
		sc->ps_flags &= ~PS_WAIT_FOR_TX_ACK;
		ath_dbg(common, PS,
			"Going back to sleep after having received TX status (0x%lx)\n",
			sc->ps_flags & (PS_WAIT_FOR_BEACON |
					PS_WAIT_FOR_CAB |
					PS_WAIT_FOR_PSPOLL_DATA |
					PS_WAIT_FOR_TX_ACK));
	}
	spin_unlock_irqrestore(&sc->sc_pm_lock, flags);

	__skb_queue_tail(&txq->complete_q, skb);
	ath_txq_skb_done(sc, txq, skb);
}
