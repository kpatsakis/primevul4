static int check_map_func_compatibility(struct bpf_map *map, int func_id)
{
	bool bool_map, bool_func;
	int i;

	if (!map)
		return 0;

	for (i = 0; i < ARRAY_SIZE(func_limit); i++) {
		bool_map = (map->map_type == func_limit[i].map_type);
		bool_func = (func_id == func_limit[i].func_id);
		/* only when map & func pair match it can continue.
		 * don't allow any other map type to be passed into
		 * the special func;
		 */
		if (bool_func && bool_map != bool_func)
			return -EINVAL;
	}

	return 0;
}
