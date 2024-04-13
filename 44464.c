static void ath_tx_fill_desc(struct ath_softc *sc, struct ath_buf *bf,
			     struct ath_txq *txq, int len)
{
	struct ath_hw *ah = sc->sc_ah;
	struct ath_buf *bf_first = NULL;
	struct ath_tx_info info;
	u32 rts_thresh = sc->hw->wiphy->rts_threshold;
	bool rts = false;

	memset(&info, 0, sizeof(info));
	info.is_first = true;
	info.is_last = true;
	info.txpower = MAX_RATE_POWER;
	info.qcu = txq->axq_qnum;

	while (bf) {
		struct sk_buff *skb = bf->bf_mpdu;
		struct ieee80211_tx_info *tx_info = IEEE80211_SKB_CB(skb);
		struct ath_frame_info *fi = get_frame_info(skb);
		bool aggr = !!(bf->bf_state.bf_type & BUF_AGGR);

		info.type = get_hw_packet_type(skb);
		if (bf->bf_next)
			info.link = bf->bf_next->bf_daddr;
		else
			info.link = (sc->tx99_state) ? bf->bf_daddr : 0;

		if (!bf_first) {
			bf_first = bf;

			if (!sc->tx99_state)
				info.flags = ATH9K_TXDESC_INTREQ;
			if ((tx_info->flags & IEEE80211_TX_CTL_CLEAR_PS_FILT) ||
			    txq == sc->tx.uapsdq)
				info.flags |= ATH9K_TXDESC_CLRDMASK;

			if (tx_info->flags & IEEE80211_TX_CTL_NO_ACK)
				info.flags |= ATH9K_TXDESC_NOACK;
			if (tx_info->flags & IEEE80211_TX_CTL_LDPC)
				info.flags |= ATH9K_TXDESC_LDPC;

			if (bf->bf_state.bfs_paprd)
				info.flags |= (u32) bf->bf_state.bfs_paprd <<
					      ATH9K_TXDESC_PAPRD_S;

			/*
			 * mac80211 doesn't handle RTS threshold for HT because
			 * the decision has to be taken based on AMPDU length
			 * and aggregation is done entirely inside ath9k.
			 * Set the RTS/CTS flag for the first subframe based
			 * on the threshold.
			 */
			if (aggr && (bf == bf_first) &&
			    unlikely(rts_thresh != (u32) -1)) {
				/*
				 * "len" is the size of the entire AMPDU.
				 */
				if (!rts_thresh || (len > rts_thresh))
					rts = true;
			}

			if (!aggr)
				len = fi->framelen;

			ath_buf_set_rate(sc, bf, &info, len, rts);
		}

		info.buf_addr[0] = bf->bf_buf_addr;
		info.buf_len[0] = skb->len;
		info.pkt_len = fi->framelen;
		info.keyix = fi->keyix;
		info.keytype = fi->keytype;

		if (aggr) {
			if (bf == bf_first)
				info.aggr = AGGR_BUF_FIRST;
			else if (bf == bf_first->bf_lastbf)
				info.aggr = AGGR_BUF_LAST;
			else
				info.aggr = AGGR_BUF_MIDDLE;

			info.ndelim = bf->bf_state.ndelim;
			info.aggr_len = len;
		}

		if (bf == bf_first->bf_lastbf)
			bf_first = NULL;

		ath9k_hw_set_txdesc(ah, bf->bf_desc, &info);
		bf = bf->bf_next;
	}
}
