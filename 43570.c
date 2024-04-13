static bool show_message(struct pool *pool, json_t *val)
{
	char *msg;

	if (!json_is_array(val))
		return false;
	msg = (char *)json_string_value(json_array_get(val, 0));
	if (!msg)
		return false;
	applog(LOG_NOTICE, "Pool %d message: %s", pool->pool_no, msg);
	return true;
}
