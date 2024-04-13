static struct btrfs_device *btrfs_find_device_by_path(
		struct btrfs_fs_info *fs_info, const char *device_path)
{
	int ret = 0;
	struct btrfs_super_block *disk_super;
	u64 devid;
	u8 *dev_uuid;
	struct block_device *bdev;
	struct btrfs_device *device;

	ret = btrfs_get_bdev_and_sb(device_path, FMODE_READ,
				    fs_info->bdev_holder, 0, &bdev, &disk_super);
	if (ret)
		return ERR_PTR(ret);

	devid = btrfs_stack_device_id(&disk_super->dev_item);
	dev_uuid = disk_super->dev_item.uuid;
	if (btrfs_fs_incompat(fs_info, METADATA_UUID))
		device = btrfs_find_device(fs_info->fs_devices, devid, dev_uuid,
					   disk_super->metadata_uuid);
	else
		device = btrfs_find_device(fs_info->fs_devices, devid, dev_uuid,
					   disk_super->fsid);

	btrfs_release_disk_super(disk_super);
	if (!device)
		device = ERR_PTR(-ENOENT);
	blkdev_put(bdev, FMODE_READ);
	return device;
}