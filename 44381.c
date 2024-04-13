void sta_info_init(struct ieee80211_local *local)
{
	spin_lock_init(&local->tim_lock);
	mutex_init(&local->sta_mtx);
	INIT_LIST_HEAD(&local->sta_list);

	setup_timer(&local->sta_cleanup, sta_info_cleanup,
		    (unsigned long)local);
}
