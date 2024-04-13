static int perf_config_bool_or_int(const char *name, const char *value, int *is_bool)
{
	*is_bool = 1;
	if (!value)
		return 1;
	if (!*value)
		return 0;
	if (!strcasecmp(value, "true") || !strcasecmp(value, "yes") || !strcasecmp(value, "on"))
		return 1;
	if (!strcasecmp(value, "false") || !strcasecmp(value, "no") || !strcasecmp(value, "off"))
		return 0;
	*is_bool = 0;
	return perf_config_int(name, value);
}
