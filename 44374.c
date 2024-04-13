static void sta_info_cleanup(unsigned long data)
{
	struct ieee80211_local *local = (struct ieee80211_local *) data;
	struct sta_info *sta;
	bool timer_needed = false;

	rcu_read_lock();
	list_for_each_entry_rcu(sta, &local->sta_list, list)
		if (sta_info_cleanup_expire_buffered(local, sta))
			timer_needed = true;
	rcu_read_unlock();

	if (local->quiescing)
		return;

	if (!timer_needed)
		return;

	mod_timer(&local->sta_cleanup,
		  round_jiffies(jiffies + STA_INFO_CLEANUP_INTERVAL));
}
