static int decide_stripe_size(struct btrfs_fs_devices *fs_devices,
			      struct alloc_chunk_ctl *ctl,
			      struct btrfs_device_info *devices_info)
{
	struct btrfs_fs_info *info = fs_devices->fs_info;

	/*
	 * Round down to number of usable stripes, devs_increment can be any
	 * number so we can't use round_down() that requires power of 2, while
	 * rounddown is safe.
	 */
	ctl->ndevs = rounddown(ctl->ndevs, ctl->devs_increment);

	if (ctl->ndevs < ctl->devs_min) {
		if (btrfs_test_opt(info, ENOSPC_DEBUG)) {
			btrfs_debug(info,
	"%s: not enough devices with free space: have=%d minimum required=%d",
				    __func__, ctl->ndevs, ctl->devs_min);
		}
		return -ENOSPC;
	}

	ctl->ndevs = min(ctl->ndevs, ctl->devs_max);

	switch (fs_devices->chunk_alloc_policy) {
	case BTRFS_CHUNK_ALLOC_REGULAR:
		return decide_stripe_size_regular(ctl, devices_info);
	case BTRFS_CHUNK_ALLOC_ZONED:
		return decide_stripe_size_zoned(ctl, devices_info);
	default:
		BUG();
	}
}