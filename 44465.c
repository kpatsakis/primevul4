ath_tx_form_aggr(struct ath_softc *sc, struct ath_txq *txq,
		 struct ath_atx_tid *tid, struct list_head *bf_q,
		 struct ath_buf *bf_first, struct sk_buff_head *tid_q,
		 int *aggr_len)
{
#define PADBYTES(_len) ((4 - ((_len) % 4)) % 4)
	struct ath_buf *bf = bf_first, *bf_prev = NULL;
	int nframes = 0, ndelim;
	u16 aggr_limit = 0, al = 0, bpad = 0,
	    al_delta, h_baw = tid->baw_size / 2;
	struct ieee80211_tx_info *tx_info;
	struct ath_frame_info *fi;
	struct sk_buff *skb;
	bool closed = false;

	bf = bf_first;
	aggr_limit = ath_lookup_rate(sc, bf, tid);

	do {
		skb = bf->bf_mpdu;
		fi = get_frame_info(skb);

		/* do not exceed aggregation limit */
		al_delta = ATH_AGGR_DELIM_SZ + fi->framelen;
		if (nframes) {
			if (aggr_limit < al + bpad + al_delta ||
			    ath_lookup_legacy(bf) || nframes >= h_baw)
				break;

			tx_info = IEEE80211_SKB_CB(bf->bf_mpdu);
			if ((tx_info->flags & IEEE80211_TX_CTL_RATE_CTRL_PROBE) ||
			    !(tx_info->flags & IEEE80211_TX_CTL_AMPDU))
				break;
		}

		/* add padding for previous frame to aggregation length */
		al += bpad + al_delta;

		/*
		 * Get the delimiters needed to meet the MPDU
		 * density for this node.
		 */
		ndelim = ath_compute_num_delims(sc, tid, bf_first, fi->framelen,
						!nframes);
		bpad = PADBYTES(al_delta) + (ndelim << 2);

		nframes++;
		bf->bf_next = NULL;

		/* link buffers of this frame to the aggregate */
		if (!fi->baw_tracked)
			ath_tx_addto_baw(sc, tid, bf);
		bf->bf_state.ndelim = ndelim;

		__skb_unlink(skb, tid_q);
		list_add_tail(&bf->list, bf_q);
		if (bf_prev)
			bf_prev->bf_next = bf;

		bf_prev = bf;

		bf = ath_tx_get_tid_subframe(sc, txq, tid, &tid_q);
		if (!bf) {
			closed = true;
			break;
		}
	} while (ath_tid_has_buffered(tid));

	bf = bf_first;
	bf->bf_lastbf = bf_prev;

	if (bf == bf_prev) {
		al = get_frame_info(bf->bf_mpdu)->framelen;
		bf->bf_state.bf_type = BUF_AMPDU;
	} else {
		TX_STAT_INC(txq->axq_qnum, a_aggr);
	}

	*aggr_len = al;

	return closed;
#undef PADBYTES
}
