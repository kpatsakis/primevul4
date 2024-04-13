void suspend_stratum(struct pool *pool)
{
	applog(LOG_INFO, "Closing socket for stratum pool %d", pool->pool_no);

	mutex_lock(&pool->stratum_lock);
	__suspend_stratum(pool);
	mutex_unlock(&pool->stratum_lock);
}
