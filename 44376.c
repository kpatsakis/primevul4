void sta_info_free(struct ieee80211_local *local, struct sta_info *sta)
{
	int i;

	if (sta->rate_ctrl)
		rate_control_free_sta(sta);

	if (sta->tx_lat) {
		for (i = 0; i < IEEE80211_NUM_TIDS; i++)
			kfree(sta->tx_lat[i].bins);
		kfree(sta->tx_lat);
	}

	sta_dbg(sta->sdata, "Destroyed STA %pM\n", sta->sta.addr);

	kfree(sta);
}
