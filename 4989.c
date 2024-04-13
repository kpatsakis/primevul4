static int chunk_usage_filter(struct btrfs_fs_info *fs_info,
		u64 chunk_offset, struct btrfs_balance_args *bargs)
{
	struct btrfs_block_group *cache;
	u64 chunk_used, user_thresh;
	int ret = 1;

	cache = btrfs_lookup_block_group(fs_info, chunk_offset);
	chunk_used = cache->used;

	if (bargs->usage_min == 0)
		user_thresh = 1;
	else if (bargs->usage > 100)
		user_thresh = cache->length;
	else
		user_thresh = div_factor_fine(cache->length, bargs->usage);

	if (chunk_used < user_thresh)
		ret = 0;

	btrfs_put_block_group(cache);
	return ret;
}