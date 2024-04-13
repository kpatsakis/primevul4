static void init_alloc_chunk_ctl_policy_zoned(
				      struct btrfs_fs_devices *fs_devices,
				      struct alloc_chunk_ctl *ctl)
{
	u64 zone_size = fs_devices->fs_info->zone_size;
	u64 limit;
	int min_num_stripes = ctl->devs_min * ctl->dev_stripes;
	int min_data_stripes = (min_num_stripes - ctl->nparity) / ctl->ncopies;
	u64 min_chunk_size = min_data_stripes * zone_size;
	u64 type = ctl->type;

	ctl->max_stripe_size = zone_size;
	if (type & BTRFS_BLOCK_GROUP_DATA) {
		ctl->max_chunk_size = round_down(BTRFS_MAX_DATA_CHUNK_SIZE,
						 zone_size);
	} else if (type & BTRFS_BLOCK_GROUP_METADATA) {
		ctl->max_chunk_size = ctl->max_stripe_size;
	} else if (type & BTRFS_BLOCK_GROUP_SYSTEM) {
		ctl->max_chunk_size = 2 * ctl->max_stripe_size;
		ctl->devs_max = min_t(int, ctl->devs_max,
				      BTRFS_MAX_DEVS_SYS_CHUNK);
	} else {
		BUG();
	}

	/* We don't want a chunk larger than 10% of writable space */
	limit = max(round_down(div_factor(fs_devices->total_rw_bytes, 1),
			       zone_size),
		    min_chunk_size);
	ctl->max_chunk_size = min(limit, ctl->max_chunk_size);
	ctl->dev_extent_min = zone_size * ctl->dev_stripes;
}