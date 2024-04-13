static bool dev_extent_hole_check_zoned(struct btrfs_device *device,
					u64 *hole_start, u64 *hole_size,
					u64 num_bytes)
{
	u64 zone_size = device->zone_info->zone_size;
	u64 pos;
	int ret;
	bool changed = false;

	ASSERT(IS_ALIGNED(*hole_start, zone_size));

	while (*hole_size > 0) {
		pos = btrfs_find_allocatable_zones(device, *hole_start,
						   *hole_start + *hole_size,
						   num_bytes);
		if (pos != *hole_start) {
			*hole_size = *hole_start + *hole_size - pos;
			*hole_start = pos;
			changed = true;
			if (*hole_size < num_bytes)
				break;
		}

		ret = btrfs_ensure_empty_zones(device, pos, num_bytes);

		/* Range is ensured to be empty */
		if (!ret)
			return changed;

		/* Given hole range was invalid (outside of device) */
		if (ret == -ERANGE) {
			*hole_start += *hole_size;
			*hole_size = 0;
			return true;
		}

		*hole_start += zone_size;
		*hole_size -= zone_size;
		changed = true;
	}

	return changed;
}