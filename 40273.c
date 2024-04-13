int lockdep_rtnl_is_held(void)
{
	return lockdep_is_held(&rtnl_mutex);
}
