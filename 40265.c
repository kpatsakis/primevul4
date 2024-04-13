void __rtnl_unlock(void)
{
	mutex_unlock(&rtnl_mutex);
}
