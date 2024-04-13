void btrfs_init_devices_late(struct btrfs_fs_info *fs_info)
{
	struct btrfs_fs_devices *fs_devices = fs_info->fs_devices, *seed_devs;
	struct btrfs_device *device;

	fs_devices->fs_info = fs_info;

	mutex_lock(&fs_devices->device_list_mutex);
	list_for_each_entry(device, &fs_devices->devices, dev_list)
		device->fs_info = fs_info;

	list_for_each_entry(seed_devs, &fs_devices->seed_list, seed_list) {
		list_for_each_entry(device, &seed_devs->devices, dev_list)
			device->fs_info = fs_info;

		seed_devs->fs_info = fs_info;
	}
	mutex_unlock(&fs_devices->device_list_mutex);
}