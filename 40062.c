	__releases(ax25_list_lock)
{
	spin_unlock_bh(&ax25_list_lock);
}
