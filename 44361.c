static void __sta_info_destroy_part2(struct sta_info *sta)
{
	struct ieee80211_local *local = sta->local;
	struct ieee80211_sub_if_data *sdata = sta->sdata;
	int ret;

	/*
	 * NOTE: This assumes at least synchronize_net() was done
	 *	 after _part1 and before _part2!
	 */

	might_sleep();
	lockdep_assert_held(&local->sta_mtx);

	/* now keys can no longer be reached */
	ieee80211_free_sta_keys(local, sta);

	sta->dead = true;

	local->num_sta--;
	local->sta_generation++;

	while (sta->sta_state > IEEE80211_STA_NONE) {
		ret = sta_info_move_state(sta, sta->sta_state - 1);
		if (ret) {
			WARN_ON_ONCE(1);
			break;
		}
	}

	if (sta->uploaded) {
		ret = drv_sta_state(local, sdata, sta, IEEE80211_STA_NONE,
				    IEEE80211_STA_NOTEXIST);
		WARN_ON_ONCE(ret != 0);
	}

	sta_dbg(sdata, "Removed STA %pM\n", sta->sta.addr);

	cfg80211_del_sta(sdata->dev, sta->sta.addr, GFP_KERNEL);

	rate_control_remove_sta_debugfs(sta);
	ieee80211_sta_debugfs_remove(sta);
	ieee80211_recalc_min_chandef(sdata);

	cleanup_single_sta(sta);
}
