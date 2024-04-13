static bool contains_pending_extent(struct btrfs_device *device, u64 *start,
				    u64 len)
{
	u64 physical_start, physical_end;

	lockdep_assert_held(&device->fs_info->chunk_mutex);

	if (!find_first_extent_bit(&device->alloc_state, *start,
				   &physical_start, &physical_end,
				   CHUNK_ALLOCATED, NULL)) {

		if (in_range(physical_start, *start, len) ||
		    in_range(*start, physical_start,
			     physical_end - physical_start)) {
			*start = physical_end + 1;
			return true;
		}
	}
	return false;
}