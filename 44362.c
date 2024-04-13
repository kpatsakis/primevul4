int __sta_info_flush(struct ieee80211_sub_if_data *sdata, bool vlans)
{
	struct ieee80211_local *local = sdata->local;
	struct sta_info *sta, *tmp;
	LIST_HEAD(free_list);
	int ret = 0;

	might_sleep();

	WARN_ON(vlans && sdata->vif.type != NL80211_IFTYPE_AP);
	WARN_ON(vlans && !sdata->bss);

	mutex_lock(&local->sta_mtx);
	list_for_each_entry_safe(sta, tmp, &local->sta_list, list) {
		if (sdata == sta->sdata ||
		    (vlans && sdata->bss == sta->sdata->bss)) {
			if (!WARN_ON(__sta_info_destroy_part1(sta)))
				list_add(&sta->free_list, &free_list);
			ret++;
		}
	}

	if (!list_empty(&free_list)) {
		synchronize_net();
		list_for_each_entry_safe(sta, tmp, &free_list, free_list)
			__sta_info_destroy_part2(sta);
	}
	mutex_unlock(&local->sta_mtx);

	return ret;
}
