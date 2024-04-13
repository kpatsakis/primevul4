void rtnl_lock(void)
{
	mutex_lock(&rtnl_mutex);
}
