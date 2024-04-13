ieee80211_tx_h_sequence(struct ieee80211_tx_data *tx)
{
	struct ieee80211_tx_info *info = IEEE80211_SKB_CB(tx->skb);
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *)tx->skb->data;
	u16 *seq;
	u8 *qc;
	int tid;

	/*
	 * Packet injection may want to control the sequence
	 * number, if we have no matching interface then we
	 * neither assign one ourselves nor ask the driver to.
	 */
	if (unlikely(info->control.vif->type == NL80211_IFTYPE_MONITOR))
		return TX_CONTINUE;

	if (unlikely(ieee80211_is_ctl(hdr->frame_control)))
		return TX_CONTINUE;

	if (ieee80211_hdrlen(hdr->frame_control) < 24)
		return TX_CONTINUE;

	if (ieee80211_is_qos_nullfunc(hdr->frame_control))
		return TX_CONTINUE;

	/*
	 * Anything but QoS data that has a sequence number field
	 * (is long enough) gets a sequence number from the global
	 * counter.  QoS data frames with a multicast destination
	 * also use the global counter (802.11-2012 9.3.2.10).
	 */
	if (!ieee80211_is_data_qos(hdr->frame_control) ||
	    is_multicast_ether_addr(hdr->addr1)) {
		/* driver should assign sequence number */
		info->flags |= IEEE80211_TX_CTL_ASSIGN_SEQ;
		/* for pure STA mode without beacons, we can do it */
		hdr->seq_ctrl = cpu_to_le16(tx->sdata->sequence_number);
		tx->sdata->sequence_number += 0x10;
		return TX_CONTINUE;
	}

	/*
	 * This should be true for injected/management frames only, for
	 * management frames we have set the IEEE80211_TX_CTL_ASSIGN_SEQ
	 * above since they are not QoS-data frames.
	 */
	if (!tx->sta)
		return TX_CONTINUE;

	/* include per-STA, per-TID sequence counter */

	qc = ieee80211_get_qos_ctl(hdr);
	tid = *qc & IEEE80211_QOS_CTL_TID_MASK;
	seq = &tx->sta->tid_seq[tid];

	hdr->seq_ctrl = cpu_to_le16(*seq);

	/* Increase the sequence number. */
	*seq = (*seq + 0x10) & IEEE80211_SCTL_SEQ;

	return TX_CONTINUE;
}
