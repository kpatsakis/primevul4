static struct btrfs_device *add_missing_dev(struct btrfs_fs_devices *fs_devices,
					    u64 devid, u8 *dev_uuid)
{
	struct btrfs_device *device;
	unsigned int nofs_flag;

	/*
	 * We call this under the chunk_mutex, so we want to use NOFS for this
	 * allocation, however we don't want to change btrfs_alloc_device() to
	 * always do NOFS because we use it in a lot of other GFP_KERNEL safe
	 * places.
	 */
	nofs_flag = memalloc_nofs_save();
	device = btrfs_alloc_device(NULL, &devid, dev_uuid);
	memalloc_nofs_restore(nofs_flag);
	if (IS_ERR(device))
		return device;

	list_add(&device->dev_list, &fs_devices->devices);
	device->fs_devices = fs_devices;
	fs_devices->num_devices++;

	set_bit(BTRFS_DEV_STATE_MISSING, &device->dev_state);
	fs_devices->missing_devices++;

	return device;
}