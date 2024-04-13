static inline int btrfs_chunk_max_errors(struct map_lookup *map)
{
	const int index = btrfs_bg_flags_to_raid_index(map->type);

	return btrfs_raid_array[index].tolerated_failures;
}