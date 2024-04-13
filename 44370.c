ieee80211_sta_ps_deliver_response(struct sta_info *sta,
				  int n_frames, u8 ignored_acs,
				  enum ieee80211_frame_release_type reason)
{
	struct ieee80211_sub_if_data *sdata = sta->sdata;
	struct ieee80211_local *local = sdata->local;
	bool more_data = false;
	int ac;
	unsigned long driver_release_tids = 0;
	struct sk_buff_head frames;

	/* Service or PS-Poll period starts */
	set_sta_flag(sta, WLAN_STA_SP);

	__skb_queue_head_init(&frames);

	/* Get response frame(s) and more data bit for the last one. */
	for (ac = 0; ac < IEEE80211_NUM_ACS; ac++) {
		unsigned long tids;

		if (ignored_acs & BIT(ac))
			continue;

		tids = ieee80211_tids_for_ac(ac);

		/* if we already have frames from software, then we can't also
		 * release from hardware queues
		 */
		if (skb_queue_empty(&frames))
			driver_release_tids |= sta->driver_buffered_tids & tids;

		if (driver_release_tids) {
			/* If the driver has data on more than one TID then
			 * certainly there's more data if we release just a
			 * single frame now (from a single TID). This will
			 * only happen for PS-Poll.
			 */
			if (reason == IEEE80211_FRAME_RELEASE_PSPOLL &&
			    hweight16(driver_release_tids) > 1) {
				more_data = true;
				driver_release_tids =
					BIT(find_highest_prio_tid(
						driver_release_tids));
				break;
			}
		} else {
			struct sk_buff *skb;

			while (n_frames > 0) {
				skb = skb_dequeue(&sta->tx_filtered[ac]);
				if (!skb) {
					skb = skb_dequeue(
						&sta->ps_tx_buf[ac]);
					if (skb)
						local->total_ps_buffered--;
				}
				if (!skb)
					break;
				n_frames--;
				__skb_queue_tail(&frames, skb);
			}
		}

		/* If we have more frames buffered on this AC, then set the
		 * more-data bit and abort the loop since we can't send more
		 * data from other ACs before the buffered frames from this.
		 */
		if (!skb_queue_empty(&sta->tx_filtered[ac]) ||
		    !skb_queue_empty(&sta->ps_tx_buf[ac])) {
			more_data = true;
			break;
		}
	}

	if (skb_queue_empty(&frames) && !driver_release_tids) {
		int tid;

		/*
		 * For PS-Poll, this can only happen due to a race condition
		 * when we set the TIM bit and the station notices it, but
		 * before it can poll for the frame we expire it.
		 *
		 * For uAPSD, this is said in the standard (11.2.1.5 h):
		 *	At each unscheduled SP for a non-AP STA, the AP shall
		 *	attempt to transmit at least one MSDU or MMPDU, but no
		 *	more than the value specified in the Max SP Length field
		 *	in the QoS Capability element from delivery-enabled ACs,
		 *	that are destined for the non-AP STA.
		 *
		 * Since we have no other MSDU/MMPDU, transmit a QoS null frame.
		 */

		/* This will evaluate to 1, 3, 5 or 7. */
		tid = 7 - ((ffs(~ignored_acs) - 1) << 1);

		ieee80211_send_null_response(sdata, sta, tid, reason, true);
	} else if (!driver_release_tids) {
		struct sk_buff_head pending;
		struct sk_buff *skb;
		int num = 0;
		u16 tids = 0;
		bool need_null = false;

		skb_queue_head_init(&pending);

		while ((skb = __skb_dequeue(&frames))) {
			struct ieee80211_tx_info *info = IEEE80211_SKB_CB(skb);
			struct ieee80211_hdr *hdr = (void *) skb->data;
			u8 *qoshdr = NULL;

			num++;

			/*
			 * Tell TX path to send this frame even though the
			 * STA may still remain is PS mode after this frame
			 * exchange.
			 */
			info->flags |= IEEE80211_TX_CTL_NO_PS_BUFFER |
				       IEEE80211_TX_CTL_PS_RESPONSE;

			/*
			 * Use MoreData flag to indicate whether there are
			 * more buffered frames for this STA
			 */
			if (more_data || !skb_queue_empty(&frames))
				hdr->frame_control |=
					cpu_to_le16(IEEE80211_FCTL_MOREDATA);
			else
				hdr->frame_control &=
					cpu_to_le16(~IEEE80211_FCTL_MOREDATA);

			if (ieee80211_is_data_qos(hdr->frame_control) ||
			    ieee80211_is_qos_nullfunc(hdr->frame_control))
				qoshdr = ieee80211_get_qos_ctl(hdr);

			tids |= BIT(skb->priority);

			__skb_queue_tail(&pending, skb);

			/* end service period after last frame or add one */
			if (!skb_queue_empty(&frames))
				continue;

			if (reason != IEEE80211_FRAME_RELEASE_UAPSD) {
				/* for PS-Poll, there's only one frame */
				info->flags |= IEEE80211_TX_STATUS_EOSP |
					       IEEE80211_TX_CTL_REQ_TX_STATUS;
				break;
			}

			/* For uAPSD, things are a bit more complicated. If the
			 * last frame has a QoS header (i.e. is a QoS-data or
			 * QoS-nulldata frame) then just set the EOSP bit there
			 * and be done.
			 * If the frame doesn't have a QoS header (which means
			 * it should be a bufferable MMPDU) then we can't set
			 * the EOSP bit in the QoS header; add a QoS-nulldata
			 * frame to the list to send it after the MMPDU.
			 *
			 * Note that this code is only in the mac80211-release
			 * code path, we assume that the driver will not buffer
			 * anything but QoS-data frames, or if it does, will
			 * create the QoS-nulldata frame by itself if needed.
			 *
			 * Cf. 802.11-2012 10.2.1.10 (c).
			 */
			if (qoshdr) {
				*qoshdr |= IEEE80211_QOS_CTL_EOSP;

				info->flags |= IEEE80211_TX_STATUS_EOSP |
					       IEEE80211_TX_CTL_REQ_TX_STATUS;
			} else {
				/* The standard isn't completely clear on this
				 * as it says the more-data bit should be set
				 * if there are more BUs. The QoS-Null frame
				 * we're about to send isn't buffered yet, we
				 * only create it below, but let's pretend it
				 * was buffered just in case some clients only
				 * expect more-data=0 when eosp=1.
				 */
				hdr->frame_control |=
					cpu_to_le16(IEEE80211_FCTL_MOREDATA);
				need_null = true;
				num++;
			}
			break;
		}

		drv_allow_buffered_frames(local, sta, tids, num,
					  reason, more_data);

		ieee80211_add_pending_skbs(local, &pending);

		if (need_null)
			ieee80211_send_null_response(
				sdata, sta, find_highest_prio_tid(tids),
				reason, false);

		sta_info_recalc_tim(sta);
	} else {
		/*
		 * We need to release a frame that is buffered somewhere in the
		 * driver ... it'll have to handle that.
		 * Note that the driver also has to check the number of frames
		 * on the TIDs we're releasing from - if there are more than
		 * n_frames it has to set the more-data bit (if we didn't ask
		 * it to set it anyway due to other buffered frames); if there
		 * are fewer than n_frames it has to make sure to adjust that
		 * to allow the service period to end properly.
		 */
		drv_release_buffered_frames(local, sta, driver_release_tids,
					    n_frames, reason, more_data);

		/*
		 * Note that we don't recalculate the TIM bit here as it would
		 * most likely have no effect at all unless the driver told us
		 * that the TID(s) became empty before returning here from the
		 * release function.
		 * Either way, however, when the driver tells us that the TID(s)
		 * became empty we'll do the TIM recalculation.
		 */
	}
}
