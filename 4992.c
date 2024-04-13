static void __btrfs_free_extra_devids(struct btrfs_fs_devices *fs_devices,
				      struct btrfs_device **latest_dev)
{
	struct btrfs_device *device, *next;

	/* This is the initialized path, it is safe to release the devices. */
	list_for_each_entry_safe(device, next, &fs_devices->devices, dev_list) {
		if (test_bit(BTRFS_DEV_STATE_IN_FS_METADATA, &device->dev_state)) {
			if (!test_bit(BTRFS_DEV_STATE_REPLACE_TGT,
				      &device->dev_state) &&
			    !test_bit(BTRFS_DEV_STATE_MISSING,
				      &device->dev_state) &&
			    (!*latest_dev ||
			     device->generation > (*latest_dev)->generation)) {
				*latest_dev = device;
			}
			continue;
		}

		/*
		 * We have already validated the presence of BTRFS_DEV_REPLACE_DEVID,
		 * in btrfs_init_dev_replace() so just continue.
		 */
		if (device->devid == BTRFS_DEV_REPLACE_DEVID)
			continue;

		if (device->bdev) {
			blkdev_put(device->bdev, device->mode);
			device->bdev = NULL;
			fs_devices->open_devices--;
		}
		if (test_bit(BTRFS_DEV_STATE_WRITEABLE, &device->dev_state)) {
			list_del_init(&device->dev_alloc_list);
			clear_bit(BTRFS_DEV_STATE_WRITEABLE, &device->dev_state);
			fs_devices->rw_devices--;
		}
		list_del_init(&device->dev_list);
		fs_devices->num_devices--;
		btrfs_free_device(device);
	}

}