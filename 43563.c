bool parse_method(struct pool *pool, char *s)
{
	json_t *val = NULL, *method, *err_val, *params;
	json_error_t err;
	bool ret = false;
	char *buf;

	if (!s)
		goto out;

	val = JSON_LOADS(s, &err);
	if (!val) {
		applog(LOG_INFO, "JSON decode failed(%d): %s", err.line, err.text);
		goto out;
	}

	method = json_object_get(val, "method");
	if (!method)
		goto out_decref;
	err_val = json_object_get(val, "error");
	params = json_object_get(val, "params");

	if (err_val && !json_is_null(err_val)) {
		char *ss;

		if (err_val)
			ss = json_dumps(err_val, JSON_INDENT(3));
		else
			ss = strdup("(unknown reason)");

		applog(LOG_INFO, "JSON-RPC method decode failed: %s", ss);
		free(ss);
		goto out_decref;
	}

	buf = (char *)json_string_value(method);
	if (!buf)
		goto out_decref;

	if (!strncasecmp(buf, "mining.notify", 13)) {
		if (parse_notify(pool, params))
			pool->stratum_notify = ret = true;
		else
			pool->stratum_notify = ret = false;
		goto out_decref;
	}

	if (!strncasecmp(buf, "mining.set_difficulty", 21)) {
		ret = parse_diff(pool, params);
		goto out_decref;
	}

	if (!strncasecmp(buf, "client.reconnect", 16)) {
		ret = parse_reconnect(pool, params);
		goto out_decref;
	}

	if (!strncasecmp(buf, "client.get_version", 18)) {
		ret =  send_version(pool, val);
		goto out_decref;
	}

	if (!strncasecmp(buf, "client.show_message", 19)) {
		ret = show_message(pool, params);
		goto out_decref;
	}
out_decref:
	json_decref(val);
out:
	return ret;
}
