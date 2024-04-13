int btrfs_repair_one_zone(struct btrfs_fs_info *fs_info, u64 logical)
{
	struct btrfs_block_group *cache;

	/* Do not attempt to repair in degraded state */
	if (btrfs_test_opt(fs_info, DEGRADED))
		return 0;

	cache = btrfs_lookup_block_group(fs_info, logical);
	if (!cache)
		return 0;

	spin_lock(&cache->lock);
	if (cache->relocating_repair) {
		spin_unlock(&cache->lock);
		btrfs_put_block_group(cache);
		return 0;
	}
	cache->relocating_repair = 1;
	spin_unlock(&cache->lock);

	kthread_run(relocating_repair_kthread, cache,
		    "btrfs-relocating-repair");

	return 0;
}