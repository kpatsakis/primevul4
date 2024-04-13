static int open_fs_devices(struct btrfs_fs_devices *fs_devices,
				fmode_t flags, void *holder)
{
	struct btrfs_device *device;
	struct btrfs_device *latest_dev = NULL;
	struct btrfs_device *tmp_device;

	flags |= FMODE_EXCL;

	list_for_each_entry_safe(device, tmp_device, &fs_devices->devices,
				 dev_list) {
		int ret;

		ret = btrfs_open_one_device(fs_devices, device, flags, holder);
		if (ret == 0 &&
		    (!latest_dev || device->generation > latest_dev->generation)) {
			latest_dev = device;
		} else if (ret == -ENODATA) {
			fs_devices->num_devices--;
			list_del(&device->dev_list);
			btrfs_free_device(device);
		}
	}
	if (fs_devices->open_devices == 0)
		return -EINVAL;

	fs_devices->opened = 1;
	fs_devices->latest_bdev = latest_dev->bdev;
	fs_devices->total_rw_bytes = 0;
	fs_devices->chunk_alloc_policy = BTRFS_CHUNK_ALLOC_REGULAR;
	fs_devices->read_policy = BTRFS_READ_POLICY_PID;

	return 0;
}