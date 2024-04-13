void ieee80211_sta_eosp(struct ieee80211_sta *pubsta)
{
	struct sta_info *sta = container_of(pubsta, struct sta_info, sta);
	struct ieee80211_local *local = sta->local;

	trace_api_eosp(local, pubsta);

	clear_sta_flag(sta, WLAN_STA_SP);
}
