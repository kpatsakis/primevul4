int perf_config_bool(const char *name, const char *value)
{
	int discard;
	return !!perf_config_bool_or_int(name, value, &discard);
}
