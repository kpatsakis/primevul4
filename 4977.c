static int btrfs_open_one_device(struct btrfs_fs_devices *fs_devices,
			struct btrfs_device *device, fmode_t flags,
			void *holder)
{
	struct request_queue *q;
	struct block_device *bdev;
	struct btrfs_super_block *disk_super;
	u64 devid;
	int ret;

	if (device->bdev)
		return -EINVAL;
	if (!device->name)
		return -EINVAL;

	ret = btrfs_get_bdev_and_sb(device->name->str, flags, holder, 1,
				    &bdev, &disk_super);
	if (ret)
		return ret;

	devid = btrfs_stack_device_id(&disk_super->dev_item);
	if (devid != device->devid)
		goto error_free_page;

	if (memcmp(device->uuid, disk_super->dev_item.uuid, BTRFS_UUID_SIZE))
		goto error_free_page;

	device->generation = btrfs_super_generation(disk_super);

	if (btrfs_super_flags(disk_super) & BTRFS_SUPER_FLAG_SEEDING) {
		if (btrfs_super_incompat_flags(disk_super) &
		    BTRFS_FEATURE_INCOMPAT_METADATA_UUID) {
			pr_err(
		"BTRFS: Invalid seeding and uuid-changed device detected\n");
			goto error_free_page;
		}

		clear_bit(BTRFS_DEV_STATE_WRITEABLE, &device->dev_state);
		fs_devices->seeding = true;
	} else {
		if (bdev_read_only(bdev))
			clear_bit(BTRFS_DEV_STATE_WRITEABLE, &device->dev_state);
		else
			set_bit(BTRFS_DEV_STATE_WRITEABLE, &device->dev_state);
	}

	q = bdev_get_queue(bdev);
	if (!blk_queue_nonrot(q))
		fs_devices->rotating = true;

	device->bdev = bdev;
	clear_bit(BTRFS_DEV_STATE_IN_FS_METADATA, &device->dev_state);
	device->mode = flags;

	fs_devices->open_devices++;
	if (test_bit(BTRFS_DEV_STATE_WRITEABLE, &device->dev_state) &&
	    device->devid != BTRFS_DEV_REPLACE_DEVID) {
		fs_devices->rw_devices++;
		list_add_tail(&device->dev_alloc_list, &fs_devices->alloc_list);
	}
	btrfs_release_disk_super(disk_super);

	return 0;

error_free_page:
	btrfs_release_disk_super(disk_super);
	blkdev_put(bdev, flags);

	return -EINVAL;
}