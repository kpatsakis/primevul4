struct sta_info *sta_info_get_by_idx(struct ieee80211_sub_if_data *sdata,
				     int idx)
{
	struct ieee80211_local *local = sdata->local;
	struct sta_info *sta;
	int i = 0;

	list_for_each_entry_rcu(sta, &local->sta_list, list) {
		if (sdata != sta->sdata)
			continue;
		if (i < idx) {
			++i;
			continue;
		}
		return sta;
	}

	return NULL;
}
