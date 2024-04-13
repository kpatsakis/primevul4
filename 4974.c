static bool is_block_group_to_copy(struct btrfs_fs_info *fs_info, u64 logical)
{
	struct btrfs_block_group *cache;
	bool ret;

	/* Non zoned filesystem does not use "to_copy" flag */
	if (!btrfs_is_zoned(fs_info))
		return false;

	cache = btrfs_lookup_block_group(fs_info, logical);

	spin_lock(&cache->lock);
	ret = cache->to_copy;
	spin_unlock(&cache->lock);

	btrfs_put_block_group(cache);
	return ret;
}