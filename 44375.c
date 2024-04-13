int sta_info_destroy_addr(struct ieee80211_sub_if_data *sdata, const u8 *addr)
{
	struct sta_info *sta;
	int ret;

	mutex_lock(&sdata->local->sta_mtx);
	sta = sta_info_get(sdata, addr);
	ret = __sta_info_destroy(sta);
	mutex_unlock(&sdata->local->sta_mtx);

	return ret;
}
