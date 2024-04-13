static void ath_tx_rc_status(struct ath_softc *sc, struct ath_buf *bf,
			     struct ath_tx_status *ts, int nframes, int nbad,
			     int txok)
{
	struct sk_buff *skb = bf->bf_mpdu;
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *)skb->data;
	struct ieee80211_tx_info *tx_info = IEEE80211_SKB_CB(skb);
	struct ieee80211_hw *hw = sc->hw;
	struct ath_hw *ah = sc->sc_ah;
	u8 i, tx_rateindex;

	if (txok)
		tx_info->status.ack_signal = ts->ts_rssi;

	tx_rateindex = ts->ts_rateindex;
	WARN_ON(tx_rateindex >= hw->max_rates);

	if (tx_info->flags & IEEE80211_TX_CTL_AMPDU) {
		tx_info->flags |= IEEE80211_TX_STAT_AMPDU;

		BUG_ON(nbad > nframes);
	}
	tx_info->status.ampdu_len = nframes;
	tx_info->status.ampdu_ack_len = nframes - nbad;

	if ((ts->ts_status & ATH9K_TXERR_FILT) == 0 &&
	    (tx_info->flags & IEEE80211_TX_CTL_NO_ACK) == 0) {
		/*
		 * If an underrun error is seen assume it as an excessive
		 * retry only if max frame trigger level has been reached
		 * (2 KB for single stream, and 4 KB for dual stream).
		 * Adjust the long retry as if the frame was tried
		 * hw->max_rate_tries times to affect how rate control updates
		 * PER for the failed rate.
		 * In case of congestion on the bus penalizing this type of
		 * underruns should help hardware actually transmit new frames
		 * successfully by eventually preferring slower rates.
		 * This itself should also alleviate congestion on the bus.
		 */
		if (unlikely(ts->ts_flags & (ATH9K_TX_DATA_UNDERRUN |
		                             ATH9K_TX_DELIM_UNDERRUN)) &&
		    ieee80211_is_data(hdr->frame_control) &&
		    ah->tx_trig_level >= sc->sc_ah->config.max_txtrig_level)
			tx_info->status.rates[tx_rateindex].count =
				hw->max_rate_tries;
	}

	for (i = tx_rateindex + 1; i < hw->max_rates; i++) {
		tx_info->status.rates[i].count = 0;
		tx_info->status.rates[i].idx = -1;
	}

	tx_info->status.rates[tx_rateindex].count = ts->ts_longretry + 1;
}
