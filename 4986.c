static u64 dev_extent_search_start(struct btrfs_device *device, u64 start)
{
	switch (device->fs_devices->chunk_alloc_policy) {
	case BTRFS_CHUNK_ALLOC_REGULAR:
		/*
		 * We don't want to overwrite the superblock on the drive nor
		 * any area used by the boot loader (grub for example), so we
		 * make sure to start at an offset of at least 1MB.
		 */
		return max_t(u64, start, SZ_1M);
	case BTRFS_CHUNK_ALLOC_ZONED:
		/*
		 * We don't care about the starting region like regular
		 * allocator, because we anyway use/reserve the first two zones
		 * for superblock logging.
		 */
		return ALIGN(start, device->zone_info->zone_size);
	default:
		BUG();
	}
}