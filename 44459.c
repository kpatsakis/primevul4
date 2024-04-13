static void ath_tx_complete_aggr(struct ath_softc *sc, struct ath_txq *txq,
				 struct ath_buf *bf, struct list_head *bf_q,
				 struct ath_tx_status *ts, int txok)
{
	struct ath_node *an = NULL;
	struct sk_buff *skb;
	struct ieee80211_sta *sta;
	struct ieee80211_hw *hw = sc->hw;
	struct ieee80211_hdr *hdr;
	struct ieee80211_tx_info *tx_info;
	struct ath_atx_tid *tid = NULL;
	struct ath_buf *bf_next, *bf_last = bf->bf_lastbf;
	struct list_head bf_head;
	struct sk_buff_head bf_pending;
	u16 seq_st = 0, acked_cnt = 0, txfail_cnt = 0, seq_first;
	u32 ba[WME_BA_BMP_SIZE >> 5];
	int isaggr, txfail, txpending, sendbar = 0, needreset = 0, nbad = 0;
	bool rc_update = true, isba;
	struct ieee80211_tx_rate rates[4];
	struct ath_frame_info *fi;
	int nframes;
	bool flush = !!(ts->ts_status & ATH9K_TX_FLUSH);
	int i, retries;
	int bar_index = -1;

	skb = bf->bf_mpdu;
	hdr = (struct ieee80211_hdr *)skb->data;

	tx_info = IEEE80211_SKB_CB(skb);

	memcpy(rates, bf->rates, sizeof(rates));

	retries = ts->ts_longretry + 1;
	for (i = 0; i < ts->ts_rateindex; i++)
		retries += rates[i].count;

	rcu_read_lock();

	sta = ieee80211_find_sta_by_ifaddr(hw, hdr->addr1, hdr->addr2);
	if (!sta) {
		rcu_read_unlock();

		INIT_LIST_HEAD(&bf_head);
		while (bf) {
			bf_next = bf->bf_next;

			if (!bf->bf_state.stale || bf_next != NULL)
				list_move_tail(&bf->list, &bf_head);

			ath_tx_complete_buf(sc, bf, txq, &bf_head, ts, 0);

			bf = bf_next;
		}
		return;
	}

	an = (struct ath_node *)sta->drv_priv;
	tid = ath_get_skb_tid(sc, an, skb);
	seq_first = tid->seq_start;
	isba = ts->ts_flags & ATH9K_TX_BA;

	/*
	 * The hardware occasionally sends a tx status for the wrong TID.
	 * In this case, the BA status cannot be considered valid and all
	 * subframes need to be retransmitted
	 *
	 * Only BlockAcks have a TID and therefore normal Acks cannot be
	 * checked
	 */
	if (isba && tid->tidno != ts->tid)
		txok = false;

	isaggr = bf_isaggr(bf);
	memset(ba, 0, WME_BA_BMP_SIZE >> 3);

	if (isaggr && txok) {
		if (ts->ts_flags & ATH9K_TX_BA) {
			seq_st = ts->ts_seqnum;
			memcpy(ba, &ts->ba_low, WME_BA_BMP_SIZE >> 3);
		} else {
			/*
			 * AR5416 can become deaf/mute when BA
			 * issue happens. Chip needs to be reset.
			 * But AP code may have sychronization issues
			 * when perform internal reset in this routine.
			 * Only enable reset in STA mode for now.
			 */
			if (sc->sc_ah->opmode == NL80211_IFTYPE_STATION)
				needreset = 1;
		}
	}

	__skb_queue_head_init(&bf_pending);

	ath_tx_count_frames(sc, bf, ts, txok, &nframes, &nbad);
	while (bf) {
		u16 seqno = bf->bf_state.seqno;

		txfail = txpending = sendbar = 0;
		bf_next = bf->bf_next;

		skb = bf->bf_mpdu;
		tx_info = IEEE80211_SKB_CB(skb);
		fi = get_frame_info(skb);

		if (!BAW_WITHIN(tid->seq_start, tid->baw_size, seqno) ||
		    !tid->active) {
			/*
			 * Outside of the current BlockAck window,
			 * maybe part of a previous session
			 */
			txfail = 1;
		} else if (ATH_BA_ISSET(ba, ATH_BA_INDEX(seq_st, seqno))) {
			/* transmit completion, subframe is
			 * acked by block ack */
			acked_cnt++;
		} else if (!isaggr && txok) {
			/* transmit completion */
			acked_cnt++;
		} else if (flush) {
			txpending = 1;
		} else if (fi->retries < ATH_MAX_SW_RETRIES) {
			if (txok || !an->sleeping)
				ath_tx_set_retry(sc, txq, bf->bf_mpdu,
						 retries);

			txpending = 1;
		} else {
			txfail = 1;
			txfail_cnt++;
			bar_index = max_t(int, bar_index,
				ATH_BA_INDEX(seq_first, seqno));
		}

		/*
		 * Make sure the last desc is reclaimed if it
		 * not a holding desc.
		 */
		INIT_LIST_HEAD(&bf_head);
		if (bf_next != NULL || !bf_last->bf_state.stale)
			list_move_tail(&bf->list, &bf_head);

		if (!txpending) {
			/*
			 * complete the acked-ones/xretried ones; update
			 * block-ack window
			 */
			ath_tx_update_baw(sc, tid, seqno);

			if (rc_update && (acked_cnt == 1 || txfail_cnt == 1)) {
				memcpy(tx_info->control.rates, rates, sizeof(rates));
				ath_tx_rc_status(sc, bf, ts, nframes, nbad, txok);
				rc_update = false;
			}

			ath_tx_complete_buf(sc, bf, txq, &bf_head, ts,
				!txfail);
		} else {
			if (tx_info->flags & IEEE80211_TX_STATUS_EOSP) {
				tx_info->flags &= ~IEEE80211_TX_STATUS_EOSP;
				ieee80211_sta_eosp(sta);
			}
			/* retry the un-acked ones */
			if (bf->bf_next == NULL && bf_last->bf_state.stale) {
				struct ath_buf *tbf;

				tbf = ath_clone_txbuf(sc, bf_last);
				/*
				 * Update tx baw and complete the
				 * frame with failed status if we
				 * run out of tx buf.
				 */
				if (!tbf) {
					ath_tx_update_baw(sc, tid, seqno);

					ath_tx_complete_buf(sc, bf, txq,
							    &bf_head, ts, 0);
					bar_index = max_t(int, bar_index,
						ATH_BA_INDEX(seq_first, seqno));
					break;
				}

				fi->bf = tbf;
			}

			/*
			 * Put this buffer to the temporary pending
			 * queue to retain ordering
			 */
			__skb_queue_tail(&bf_pending, skb);
		}

		bf = bf_next;
	}

	/* prepend un-acked frames to the beginning of the pending frame queue */
	if (!skb_queue_empty(&bf_pending)) {
		if (an->sleeping)
			ieee80211_sta_set_buffered(sta, tid->tidno, true);

		skb_queue_splice_tail(&bf_pending, &tid->retry_q);
		if (!an->sleeping) {
			ath_tx_queue_tid(txq, tid);

			if (ts->ts_status & (ATH9K_TXERR_FILT | ATH9K_TXERR_XRETRY))
				tid->ac->clear_ps_filter = true;
		}
	}

	if (bar_index >= 0) {
		u16 bar_seq = ATH_BA_INDEX2SEQ(seq_first, bar_index);

		if (BAW_WITHIN(tid->seq_start, tid->baw_size, bar_seq))
			tid->bar_index = ATH_BA_INDEX(tid->seq_start, bar_seq);

		ath_txq_unlock(sc, txq);
		ath_send_bar(tid, ATH_BA_INDEX2SEQ(seq_first, bar_index + 1));
		ath_txq_lock(sc, txq);
	}

	rcu_read_unlock();

	if (needreset)
		ath9k_queue_reset(sc, RESET_TYPE_TX_ERROR);
}
