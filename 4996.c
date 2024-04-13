int btrfs_chunk_readonly(struct btrfs_fs_info *fs_info, u64 chunk_offset)
{
	struct extent_map *em;
	struct map_lookup *map;
	int readonly = 0;
	int miss_ndevs = 0;
	int i;

	em = btrfs_get_chunk_map(fs_info, chunk_offset, 1);
	if (IS_ERR(em))
		return 1;

	map = em->map_lookup;
	for (i = 0; i < map->num_stripes; i++) {
		if (test_bit(BTRFS_DEV_STATE_MISSING,
					&map->stripes[i].dev->dev_state)) {
			miss_ndevs++;
			continue;
		}
		if (!test_bit(BTRFS_DEV_STATE_WRITEABLE,
					&map->stripes[i].dev->dev_state)) {
			readonly = 1;
			goto end;
		}
	}

	/*
	 * If the number of missing devices is larger than max errors,
	 * we can not write the data into that chunk successfully, so
	 * set it readonly.
	 */
	if (miss_ndevs > btrfs_chunk_max_errors(map))
		readonly = 1;
end:
	free_extent_map(em);
	return readonly;
}