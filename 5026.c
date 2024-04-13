static struct btrfs_device * btrfs_find_next_active_device(
		struct btrfs_fs_devices *fs_devs, struct btrfs_device *device)
{
	struct btrfs_device *next_device;

	list_for_each_entry(next_device, &fs_devs->devices, dev_list) {
		if (next_device != device &&
		    !test_bit(BTRFS_DEV_STATE_MISSING, &next_device->dev_state)
		    && next_device->bdev)
			return next_device;
	}

	return NULL;
}