void sta_info_stop(struct ieee80211_local *local)
{
	del_timer_sync(&local->sta_cleanup);
}
