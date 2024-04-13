static void close_fs_devices(struct btrfs_fs_devices *fs_devices)
{
	struct btrfs_device *device, *tmp;

	lockdep_assert_held(&uuid_mutex);

	if (--fs_devices->opened > 0)
		return;

	list_for_each_entry_safe(device, tmp, &fs_devices->devices, dev_list)
		btrfs_close_one_device(device);

	WARN_ON(fs_devices->open_devices);
	WARN_ON(fs_devices->rw_devices);
	fs_devices->opened = 0;
	fs_devices->seeding = false;
	fs_devices->fs_info = NULL;
}