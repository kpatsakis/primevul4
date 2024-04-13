int btrfs_open_devices(struct btrfs_fs_devices *fs_devices,
		       fmode_t flags, void *holder)
{
	int ret;

	lockdep_assert_held(&uuid_mutex);
	/*
	 * The device_list_mutex cannot be taken here in case opening the
	 * underlying device takes further locks like open_mutex.
	 *
	 * We also don't need the lock here as this is called during mount and
	 * exclusion is provided by uuid_mutex
	 */

	if (fs_devices->opened) {
		fs_devices->opened++;
		ret = 0;
	} else {
		list_sort(NULL, &fs_devices->devices, devid_cmp);
		ret = open_fs_devices(fs_devices, flags, holder);
	}

	return ret;
}