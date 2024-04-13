int rtnl_trylock(void)
{
	return mutex_trylock(&rtnl_mutex);
}
