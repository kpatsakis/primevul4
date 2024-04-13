static void btrfs_close_one_device(struct btrfs_device *device)
{
	struct btrfs_fs_devices *fs_devices = device->fs_devices;

	if (test_bit(BTRFS_DEV_STATE_WRITEABLE, &device->dev_state) &&
	    device->devid != BTRFS_DEV_REPLACE_DEVID) {
		list_del_init(&device->dev_alloc_list);
		fs_devices->rw_devices--;
	}

	if (test_bit(BTRFS_DEV_STATE_MISSING, &device->dev_state))
		fs_devices->missing_devices--;

	btrfs_close_bdev(device);
	if (device->bdev) {
		fs_devices->open_devices--;
		device->bdev = NULL;
	}
	clear_bit(BTRFS_DEV_STATE_WRITEABLE, &device->dev_state);
	btrfs_destroy_dev_zone_info(device);

	device->fs_info = NULL;
	atomic_set(&device->dev_stats_ccnt, 0);
	extent_io_tree_release(&device->alloc_state);

	/* Verify the device is back in a pristine state  */
	ASSERT(!test_bit(BTRFS_DEV_STATE_FLUSH_SENT, &device->dev_state));
	ASSERT(!test_bit(BTRFS_DEV_STATE_REPLACE_TGT, &device->dev_state));
	ASSERT(list_empty(&device->dev_alloc_list));
	ASSERT(list_empty(&device->post_commit_list));
	ASSERT(atomic_read(&device->reada_in_flight) == 0);
}