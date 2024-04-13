static void ax25_cb_del(ax25_cb *ax25)
{
	if (!hlist_unhashed(&ax25->ax25_node)) {
		spin_lock_bh(&ax25_list_lock);
		hlist_del_init(&ax25->ax25_node);
		spin_unlock_bh(&ax25_list_lock);
		ax25_cb_put(ax25);
	}
}
