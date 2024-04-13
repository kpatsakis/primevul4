static bool parse_diff(struct pool *pool, json_t *val)
{
	double old_diff, diff;

	diff = json_number_value(json_array_get(val, 0));
	if (diff == 0)
		return false;

	cg_wlock(&pool->data_lock);
	old_diff = pool->sdiff;
	pool->sdiff = diff;
	cg_wunlock(&pool->data_lock);

	if (old_diff != diff) {
		int idiff = diff;

		if ((double)idiff == diff)
			applog(LOG_NOTICE, "Pool %d difficulty changed to %d",
			       pool->pool_no, idiff);
		else
			applog(LOG_NOTICE, "Pool %d difficulty changed to %.1f",
			       pool->pool_no, diff);
	} else
		applog(LOG_DEBUG, "Pool %d difficulty set to %f", pool->pool_no,
		       diff);

	return true;
}
