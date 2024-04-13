static void btrfs_close_bdev(struct btrfs_device *device)
{
	if (!device->bdev)
		return;

	if (test_bit(BTRFS_DEV_STATE_WRITEABLE, &device->dev_state)) {
		sync_blockdev(device->bdev);
		invalidate_bdev(device->bdev);
	}

	blkdev_put(device->bdev, device->mode);
}