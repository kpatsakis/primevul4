static void purge_old_ps_buffers(struct ieee80211_local *local)
{
	int total = 0, purged = 0;
	struct sk_buff *skb;
	struct ieee80211_sub_if_data *sdata;
	struct sta_info *sta;

	list_for_each_entry_rcu(sdata, &local->interfaces, list) {
		struct ps_data *ps;

		if (sdata->vif.type == NL80211_IFTYPE_AP)
			ps = &sdata->u.ap.ps;
		else if (ieee80211_vif_is_mesh(&sdata->vif))
			ps = &sdata->u.mesh.ps;
		else
			continue;

		skb = skb_dequeue(&ps->bc_buf);
		if (skb) {
			purged++;
			dev_kfree_skb(skb);
		}
		total += skb_queue_len(&ps->bc_buf);
	}

	/*
	 * Drop one frame from each station from the lowest-priority
	 * AC that has frames at all.
	 */
	list_for_each_entry_rcu(sta, &local->sta_list, list) {
		int ac;

		for (ac = IEEE80211_AC_BK; ac >= IEEE80211_AC_VO; ac--) {
			skb = skb_dequeue(&sta->ps_tx_buf[ac]);
			total += skb_queue_len(&sta->ps_tx_buf[ac]);
			if (skb) {
				purged++;
				ieee80211_free_txskb(&local->hw, skb);
				break;
			}
		}
	}

	local->total_ps_buffered = total;
	ps_dbg_hw(&local->hw, "PS buffers full - purged %d frames\n", purged);
}
