int btrfs_is_parity_mirror(struct btrfs_fs_info *fs_info, u64 logical, u64 len)
{
	struct extent_map *em;
	struct map_lookup *map;
	int ret = 0;

	em = btrfs_get_chunk_map(fs_info, logical, len);

	if(!WARN_ON(IS_ERR(em))) {
		map = em->map_lookup;
		if (map->type & BTRFS_BLOCK_GROUP_RAID56_MASK)
			ret = 1;
		free_extent_map(em);
	}
	return ret;
}