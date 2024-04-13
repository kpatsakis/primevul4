void __cold btrfs_assign_next_active_device(struct btrfs_device *device,
					    struct btrfs_device *next_device)
{
	struct btrfs_fs_info *fs_info = device->fs_info;

	if (!next_device)
		next_device = btrfs_find_next_active_device(fs_info->fs_devices,
							    device);
	ASSERT(next_device);

	if (fs_info->sb->s_bdev &&
			(fs_info->sb->s_bdev == device->bdev))
		fs_info->sb->s_bdev = next_device->bdev;

	if (fs_info->fs_devices->latest_bdev == device->bdev)
		fs_info->fs_devices->latest_bdev = next_device->bdev;
}