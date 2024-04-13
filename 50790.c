bool lockdep_nfnl_is_held(u8 subsys_id)
{
	return lockdep_is_held(&table[subsys_id].mutex);
}
