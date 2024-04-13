bool restart_stratum(struct pool *pool)
{
	applog(LOG_DEBUG, "Restarting stratum on pool %s", get_pool_name(pool));

	if (pool->stratum_active)
		suspend_stratum(pool);
	if (!initiate_stratum(pool))
		return false;
	if (pool->extranonce_subscribe && !subscribe_extranonce(pool))
		return false;
	if (!auth_stratum(pool))
		return false;

	return true;
}
