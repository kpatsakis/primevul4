ieee80211_tx_h_multicast_ps_buf(struct ieee80211_tx_data *tx)
{
	struct ieee80211_tx_info *info = IEEE80211_SKB_CB(tx->skb);
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *)tx->skb->data;
	struct ps_data *ps;

	/*
	 * broadcast/multicast frame
	 *
	 * If any of the associated/peer stations is in power save mode,
	 * the frame is buffered to be sent after DTIM beacon frame.
	 * This is done either by the hardware or us.
	 */

	/* powersaving STAs currently only in AP/VLAN/mesh mode */
	if (tx->sdata->vif.type == NL80211_IFTYPE_AP ||
	    tx->sdata->vif.type == NL80211_IFTYPE_AP_VLAN) {
		if (!tx->sdata->bss)
			return TX_CONTINUE;

		ps = &tx->sdata->bss->ps;
	} else if (ieee80211_vif_is_mesh(&tx->sdata->vif)) {
		ps = &tx->sdata->u.mesh.ps;
	} else {
		return TX_CONTINUE;
	}


	/* no buffering for ordered frames */
	if (ieee80211_has_order(hdr->frame_control))
		return TX_CONTINUE;

	if (tx->local->hw.flags & IEEE80211_HW_QUEUE_CONTROL)
		info->hw_queue = tx->sdata->vif.cab_queue;

	/* no stations in PS mode */
	if (!atomic_read(&ps->num_sta_ps))
		return TX_CONTINUE;

	info->flags |= IEEE80211_TX_CTL_SEND_AFTER_DTIM;

	/* device releases frame after DTIM beacon */
	if (!(tx->local->hw.flags & IEEE80211_HW_HOST_BROADCAST_PS_BUFFERING))
		return TX_CONTINUE;

	/* buffered in mac80211 */
	if (tx->local->total_ps_buffered >= TOTAL_MAX_TX_BUFFER)
		purge_old_ps_buffers(tx->local);

	if (skb_queue_len(&ps->bc_buf) >= AP_MAX_BC_BUFFER) {
		ps_dbg(tx->sdata,
		       "BC TX buffer full - dropping the oldest frame\n");
		dev_kfree_skb(skb_dequeue(&ps->bc_buf));
	} else
		tx->local->total_ps_buffered++;

	skb_queue_tail(&ps->bc_buf, tx->skb);

	return TX_QUEUED;
}
