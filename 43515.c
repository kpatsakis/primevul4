bool parse_method(struct pool *pool, char *s)
{
	json_t *val = NULL, *method, *err_val, *params;
	json_error_t err;
	bool ret = false;
	char *buf;

	if (!s)
		return ret;

	val = JSON_LOADS(s, &err);
	if (!val) {
		applog(LOG_INFO, "JSON decode failed(%d): %s", err.line, err.text);
		return ret;
	}

	method = json_object_get(val, "method");
	if (!method) {
		json_decref(val);
		return ret;
	}
	err_val = json_object_get(val, "error");
	params = json_object_get(val, "params");

	if (err_val && !json_is_null(err_val)) {
		char *ss;

		if (err_val)
			ss = json_dumps(err_val, JSON_INDENT(3));
		else
			ss = strdup("(unknown reason)");

		applog(LOG_INFO, "JSON-RPC method decode failed: %s", ss);

		json_decref(val);
		free(ss);

		return ret;
	}

	buf = (char *)json_string_value(method);
	if (!buf) {
		json_decref(val);
		return ret;
	}

	if (!strncasecmp(buf, "mining.notify", 13)) {
		if (parse_notify(pool, params))
			pool->stratum_notify = ret = true;
		else
			pool->stratum_notify = ret = false;
		json_decref(val);
		return ret;
	}

	if (!strncasecmp(buf, "mining.set_difficulty", 21) && parse_diff(pool, params)) {
		ret = true;
		json_decref(val);
		return ret;
	}

	if (!strncasecmp(buf, "mining.set_extranonce", 21) && parse_extranonce(pool, params)) {
		ret = true;
		json_decref(val);
		return ret;
	}

	if (!strncasecmp(buf, "client.reconnect", 16) && parse_reconnect(pool, params)) {
		ret = true;
		json_decref(val);
		return ret;
	}

	if (!strncasecmp(buf, "client.get_version", 18) && send_version(pool, val)) {
		ret = true;
		json_decref(val);
		return ret;
	}

	if (!strncasecmp(buf, "client.show_message", 19) && show_message(pool, params)) {
		ret = true;
		json_decref(val);
		return ret;
	}
	json_decref(val);
	return ret;
}
