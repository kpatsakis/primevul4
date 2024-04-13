static void init_alloc_chunk_ctl_policy_regular(
				struct btrfs_fs_devices *fs_devices,
				struct alloc_chunk_ctl *ctl)
{
	u64 type = ctl->type;

	if (type & BTRFS_BLOCK_GROUP_DATA) {
		ctl->max_stripe_size = SZ_1G;
		ctl->max_chunk_size = BTRFS_MAX_DATA_CHUNK_SIZE;
	} else if (type & BTRFS_BLOCK_GROUP_METADATA) {
		/* For larger filesystems, use larger metadata chunks */
		if (fs_devices->total_rw_bytes > 50ULL * SZ_1G)
			ctl->max_stripe_size = SZ_1G;
		else
			ctl->max_stripe_size = SZ_256M;
		ctl->max_chunk_size = ctl->max_stripe_size;
	} else if (type & BTRFS_BLOCK_GROUP_SYSTEM) {
		ctl->max_stripe_size = SZ_32M;
		ctl->max_chunk_size = 2 * ctl->max_stripe_size;
		ctl->devs_max = min_t(int, ctl->devs_max,
				      BTRFS_MAX_DEVS_SYS_CHUNK);
	} else {
		BUG();
	}

	/* We don't want a chunk larger than 10% of writable space */
	ctl->max_chunk_size = min(div_factor(fs_devices->total_rw_bytes, 1),
				  ctl->max_chunk_size);
	ctl->dev_extent_min = BTRFS_STRIPE_LEN * ctl->dev_stripes;
}