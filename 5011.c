static bool dev_extent_hole_check(struct btrfs_device *device, u64 *hole_start,
				  u64 *hole_size, u64 num_bytes)
{
	bool changed = false;
	u64 hole_end = *hole_start + *hole_size;

	for (;;) {
		/*
		 * Check before we set max_hole_start, otherwise we could end up
		 * sending back this offset anyway.
		 */
		if (contains_pending_extent(device, hole_start, *hole_size)) {
			if (hole_end >= *hole_start)
				*hole_size = hole_end - *hole_start;
			else
				*hole_size = 0;
			changed = true;
		}

		switch (device->fs_devices->chunk_alloc_policy) {
		case BTRFS_CHUNK_ALLOC_REGULAR:
			/* No extra check */
			break;
		case BTRFS_CHUNK_ALLOC_ZONED:
			if (dev_extent_hole_check_zoned(device, hole_start,
							hole_size, num_bytes)) {
				changed = true;
				/*
				 * The changed hole can contain pending extent.
				 * Loop again to check that.
				 */
				continue;
			}
			break;
		default:
			BUG();
		}

		break;
	}

	return changed;
}