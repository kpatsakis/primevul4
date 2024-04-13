static bool parse_diff(struct pool *pool, json_t *val)
{
	double old_diff, diff;

	if (opt_diff_mult == 0.0)
		diff = json_number_value(json_array_get(val, 0)) * pool->algorithm.diff_multiplier1;
	else
		diff = json_number_value(json_array_get(val, 0)) * opt_diff_mult;

	if (diff == 0)
		return false;

	cg_wlock(&pool->data_lock);
	old_diff = pool->swork.diff;
	pool->swork.diff = diff;
	cg_wunlock(&pool->data_lock);

	if (old_diff != diff) {
		int idiff = diff;

		if ((double)idiff == diff)
			applog(pool == current_pool() ? LOG_NOTICE : LOG_DEBUG, "%s difficulty changed to %d", get_pool_name(pool), idiff);
		else
			applog(pool == current_pool() ? LOG_NOTICE : LOG_DEBUG, "%s difficulty changed to %.3f", get_pool_name(pool), diff);
	} else
		applog(LOG_DEBUG, "%s difficulty set to %f", get_pool_name(pool), diff);

	return true;
}
