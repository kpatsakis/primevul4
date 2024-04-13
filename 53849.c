static int map_str_to_val(const struct aiptek_map *map, const char *str, size_t count)
{
	const struct aiptek_map *p;

	if (str[count - 1] == '\n')
		count--;

	for (p = map; p->string; p++)
	        if (!strncmp(str, p->string, count))
			return p->value;

	return AIPTEK_INVALID_VALUE;
}
