static char *get_sessionid(json_t *val)
{
	char *ret = NULL;
	json_t *arr_val;
	int arrsize, i;

	arr_val = json_array_get(val, 0);
	if (!arr_val || !json_is_array(arr_val))
		goto out;
	arrsize = json_array_size(arr_val);
	for (i = 0; i < arrsize; i++) {
		json_t *arr = json_array_get(arr_val, i);
		char *notify;

		if (!arr | !json_is_array(arr))
			break;
		notify = __json_array_string(arr, 0);
		if (!notify)
			continue;
		if (!strncasecmp(notify, "mining.notify", 13)) {
			ret = json_array_string(arr, 1);
			break;
		}
	}
out:
	return ret;
}
