static const char *map_val_to_str(const struct aiptek_map *map, int val)
{
	const struct aiptek_map *p;

	for (p = map; p->value != AIPTEK_INVALID_VALUE; p++)
		if (val == p->value)
			return p->string;

	return "unknown";
}
