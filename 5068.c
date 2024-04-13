static struct btrfs_fs_devices *open_seed_devices(struct btrfs_fs_info *fs_info,
						  u8 *fsid)
{
	struct btrfs_fs_devices *fs_devices;
	int ret;

	lockdep_assert_held(&uuid_mutex);
	ASSERT(fsid);

	/* This will match only for multi-device seed fs */
	list_for_each_entry(fs_devices, &fs_info->fs_devices->seed_list, seed_list)
		if (!memcmp(fs_devices->fsid, fsid, BTRFS_FSID_SIZE))
			return fs_devices;


	fs_devices = find_fsid(fsid, NULL);
	if (!fs_devices) {
		if (!btrfs_test_opt(fs_info, DEGRADED))
			return ERR_PTR(-ENOENT);

		fs_devices = alloc_fs_devices(fsid, NULL);
		if (IS_ERR(fs_devices))
			return fs_devices;

		fs_devices->seeding = true;
		fs_devices->opened = 1;
		return fs_devices;
	}

	/*
	 * Upon first call for a seed fs fsid, just create a private copy of the
	 * respective fs_devices and anchor it at fs_info->fs_devices->seed_list
	 */
	fs_devices = clone_fs_devices(fs_devices);
	if (IS_ERR(fs_devices))
		return fs_devices;

	ret = open_fs_devices(fs_devices, FMODE_READ, fs_info->bdev_holder);
	if (ret) {
		free_fs_devices(fs_devices);
		return ERR_PTR(ret);
	}

	if (!fs_devices->seeding) {
		close_fs_devices(fs_devices);
		free_fs_devices(fs_devices);
		return ERR_PTR(-EINVAL);
	}

	list_add(&fs_devices->seed_list, &fs_info->fs_devices->seed_list);

	return fs_devices;
}