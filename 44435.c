int ath9k_tx99_send(struct ath_softc *sc, struct sk_buff *skb,
		    struct ath_tx_control *txctl)
{
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *) skb->data;
	struct ath_frame_info *fi = get_frame_info(skb);
	struct ath_common *common = ath9k_hw_common(sc->sc_ah);
	struct ath_buf *bf;
	int padpos, padsize;

	padpos = ieee80211_hdrlen(hdr->frame_control);
	padsize = padpos & 3;

	if (padsize && skb->len > padpos) {
		if (skb_headroom(skb) < padsize) {
			ath_dbg(common, XMIT,
				"tx99 padding failed\n");
		return -EINVAL;
		}

		skb_push(skb, padsize);
		memmove(skb->data, skb->data + padsize, padpos);
	}

	fi->keyix = ATH9K_TXKEYIX_INVALID;
	fi->framelen = skb->len + FCS_LEN;
	fi->keytype = ATH9K_KEY_TYPE_CLEAR;

	bf = ath_tx_setup_buffer(sc, txctl->txq, NULL, skb);
	if (!bf) {
		ath_dbg(common, XMIT, "tx99 buffer setup failed\n");
		return -EINVAL;
	}

	ath_set_rates(sc->tx99_vif, NULL, bf);

	ath9k_hw_set_desc_link(sc->sc_ah, bf->bf_desc, bf->bf_daddr);
	ath9k_hw_tx99_start(sc->sc_ah, txctl->txq->axq_qnum);

	ath_tx_send_normal(sc, txctl->txq, NULL, skb);

	return 0;
}
