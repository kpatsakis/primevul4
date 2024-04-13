void ieee80211_sta_expire(struct ieee80211_sub_if_data *sdata,
			  unsigned long exp_time)
{
	struct ieee80211_local *local = sdata->local;
	struct sta_info *sta, *tmp;

	mutex_lock(&local->sta_mtx);

	list_for_each_entry_safe(sta, tmp, &local->sta_list, list) {
		if (sdata != sta->sdata)
			continue;

		if (time_after(jiffies, sta->last_rx + exp_time)) {
			sta_dbg(sta->sdata, "expiring inactive STA %pM\n",
				sta->sta.addr);

			if (ieee80211_vif_is_mesh(&sdata->vif) &&
			    test_sta_flag(sta, WLAN_STA_PS_STA))
				atomic_dec(&sdata->u.mesh.ps.num_sta_ps);

			WARN_ON(__sta_info_destroy(sta));
		}
	}

	mutex_unlock(&local->sta_mtx);
}
