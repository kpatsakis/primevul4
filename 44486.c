static void ath_txq_skb_done(struct ath_softc *sc, struct ath_txq *txq,
			     struct sk_buff *skb)
{
	int q;

	q = skb_get_queue_mapping(skb);
	if (txq == sc->tx.uapsdq)
		txq = sc->tx.txq_map[q];

	if (txq != sc->tx.txq_map[q])
		return;

	if (WARN_ON(--txq->pending_frames < 0))
		txq->pending_frames = 0;

	if (txq->stopped &&
	    txq->pending_frames < sc->tx.txq_max_pending[q]) {
		ieee80211_wake_queue(sc->hw, q);
		txq->stopped = false;
	}
}
