static int perf_parse_long(const char *value, long *ret)
{
	if (value && *value) {
		char *end;
		long val = strtol(value, &end, 0);
		unsigned long factor = 1;
		if (!parse_unit_factor(end, &factor))
			return 0;
		*ret = val * factor;
		return 1;
	}
	return 0;
}
