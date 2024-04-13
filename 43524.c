static bool send_version(struct pool *pool, json_t *val)
{
	char s[RBUFSIZE];
	int id = json_integer_value(json_object_get(val, "id"));

	if (!id)
		return false;

	sprintf(s, "{\"id\": %d, \"result\": \""PACKAGE"/"VERSION"\", \"error\": null}", id);
	if (!stratum_send(pool, s, strlen(s)))
		return false;

	return true;
}
