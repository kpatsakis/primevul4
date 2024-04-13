	__acquires(ax25_list_lock)
{
	spin_lock_bh(&ax25_list_lock);
	return seq_hlist_start(&ax25_list, *pos);
}
