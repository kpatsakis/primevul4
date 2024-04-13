int btrfs_init_dev_stats(struct btrfs_fs_info *fs_info)
{
	struct btrfs_fs_devices *fs_devices = fs_info->fs_devices, *seed_devs;
	struct btrfs_device *device;
	struct btrfs_path *path = NULL;
	int ret = 0;

	path = btrfs_alloc_path();
	if (!path)
		return -ENOMEM;

	mutex_lock(&fs_devices->device_list_mutex);
	list_for_each_entry(device, &fs_devices->devices, dev_list) {
		ret = btrfs_device_init_dev_stats(device, path);
		if (ret)
			goto out;
	}
	list_for_each_entry(seed_devs, &fs_devices->seed_list, seed_list) {
		list_for_each_entry(device, &seed_devs->devices, dev_list) {
			ret = btrfs_device_init_dev_stats(device, path);
			if (ret)
				goto out;
		}
	}
out:
	mutex_unlock(&fs_devices->device_list_mutex);

	btrfs_free_path(path);
	return ret;
}