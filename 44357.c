int sta_info_insert_rcu(struct sta_info *sta) __acquires(RCU)
{
	struct ieee80211_local *local = sta->local;
	int err = 0;

	might_sleep();

	err = sta_info_insert_check(sta);
	if (err) {
		rcu_read_lock();
		goto out_free;
	}

	mutex_lock(&local->sta_mtx);

	err = sta_info_insert_finish(sta);
	if (err)
		goto out_free;

	return 0;
 out_free:
	BUG_ON(!err);
	sta_info_free(local, sta);
	return err;
}
