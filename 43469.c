bool auth_stratum(struct pool *pool)
{
	json_t *val = NULL, *res_val, *err_val;
	char s[RBUFSIZE], *sret = NULL;
	json_error_t err;
	bool ret = false;

	sprintf(s, "{\"id\": %d, \"method\": \"mining.authorize\", \"params\": [\"%s\", \"%s\"]}",
		swork_id++, pool->rpc_user, pool->rpc_pass);

	if (!stratum_send(pool, s, strlen(s)))
		return ret;

	/* Parse all data in the queue and anything left should be auth */
	while (42) {
		sret = recv_line(pool);
		if (!sret)
			return ret;
		if (parse_method(pool, sret))
			free(sret);
		else
			break;
	}

	val = JSON_LOADS(sret, &err);
	free(sret);
	res_val = json_object_get(val, "result");
	err_val = json_object_get(val, "error");

	if (!res_val || json_is_false(res_val) || (err_val && !json_is_null(err_val)))  {
		char *ss;

		if (err_val)
			ss = json_dumps(err_val, JSON_INDENT(3));
		else
			ss = strdup("(unknown reason)");
		applog(LOG_INFO, "%s JSON stratum auth failed: %s", get_pool_name(pool), ss);
		free(ss);

		suspend_stratum(pool);

		goto out;
	}

	ret = true;
	applog(LOG_INFO, "Stratum authorisation success for %s", get_pool_name(pool));
	pool->probed = true;
	successful_connect = true;

out:
	json_decref(val);
	return ret;
}
