void ax25_cb_add(ax25_cb *ax25)
{
	spin_lock_bh(&ax25_list_lock);
	ax25_cb_hold(ax25);
	hlist_add_head(&ax25->ax25_node, &ax25_list);
	spin_unlock_bh(&ax25_list_lock);
}
