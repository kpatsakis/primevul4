void ieee80211_sta_set_buffered(struct ieee80211_sta *pubsta,
				u8 tid, bool buffered)
{
	struct sta_info *sta = container_of(pubsta, struct sta_info, sta);

	if (WARN_ON(tid >= IEEE80211_NUM_TIDS))
		return;

	trace_api_sta_set_buffered(sta->local, pubsta, tid, buffered);

	if (buffered)
		set_bit(tid, &sta->driver_buffered_tids);
	else
		clear_bit(tid, &sta->driver_buffered_tids);

	sta_info_recalc_tim(sta);
}
