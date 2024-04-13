static int relocating_repair_kthread(void *data)
{
	struct btrfs_block_group *cache = (struct btrfs_block_group *)data;
	struct btrfs_fs_info *fs_info = cache->fs_info;
	u64 target;
	int ret = 0;

	target = cache->start;
	btrfs_put_block_group(cache);

	if (!btrfs_exclop_start(fs_info, BTRFS_EXCLOP_BALANCE)) {
		btrfs_info(fs_info,
			   "zoned: skip relocating block group %llu to repair: EBUSY",
			   target);
		return -EBUSY;
	}

	mutex_lock(&fs_info->reclaim_bgs_lock);

	/* Ensure block group still exists */
	cache = btrfs_lookup_block_group(fs_info, target);
	if (!cache)
		goto out;

	if (!cache->relocating_repair)
		goto out;

	ret = btrfs_may_alloc_data_chunk(fs_info, target);
	if (ret < 0)
		goto out;

	btrfs_info(fs_info,
		   "zoned: relocating block group %llu to repair IO failure",
		   target);
	ret = btrfs_relocate_chunk(fs_info, target);

out:
	if (cache)
		btrfs_put_block_group(cache);
	mutex_unlock(&fs_info->reclaim_bgs_lock);
	btrfs_exclop_finish(fs_info);

	return ret;
}