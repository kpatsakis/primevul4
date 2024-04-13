bool restart_stratum(struct pool *pool)
{
	if (pool->stratum_active)
		suspend_stratum(pool);
	if (!initiate_stratum(pool))
		return false;
	if (!auth_stratum(pool))
		return false;
	return true;
}
