void nfnl_lock(__u8 subsys_id)
{
	mutex_lock(&table[subsys_id].mutex);
}
