int sta_info_insert(struct sta_info *sta)
{
	int err = sta_info_insert_rcu(sta);

	rcu_read_unlock();

	return err;
}
