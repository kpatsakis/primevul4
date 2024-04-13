unsigned long btrfs_full_stripe_len(struct btrfs_fs_info *fs_info,
				    u64 logical)
{
	struct extent_map *em;
	struct map_lookup *map;
	unsigned long len = fs_info->sectorsize;

	em = btrfs_get_chunk_map(fs_info, logical, len);

	if (!WARN_ON(IS_ERR(em))) {
		map = em->map_lookup;
		if (map->type & BTRFS_BLOCK_GROUP_RAID56_MASK)
			len = map->stripe_len * nr_data_stripes(map);
		free_extent_map(em);
	}
	return len;
}