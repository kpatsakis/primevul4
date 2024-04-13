void btrfs_free_extra_devids(struct btrfs_fs_devices *fs_devices)
{
	struct btrfs_device *latest_dev = NULL;
	struct btrfs_fs_devices *seed_dev;

	mutex_lock(&uuid_mutex);
	__btrfs_free_extra_devids(fs_devices, &latest_dev);

	list_for_each_entry(seed_dev, &fs_devices->seed_list, seed_list)
		__btrfs_free_extra_devids(seed_dev, &latest_dev);

	fs_devices->latest_bdev = latest_dev->bdev;

	mutex_unlock(&uuid_mutex);
}