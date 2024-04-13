static int gather_device_info(struct btrfs_fs_devices *fs_devices,
			      struct alloc_chunk_ctl *ctl,
			      struct btrfs_device_info *devices_info)
{
	struct btrfs_fs_info *info = fs_devices->fs_info;
	struct btrfs_device *device;
	u64 total_avail;
	u64 dev_extent_want = ctl->max_stripe_size * ctl->dev_stripes;
	int ret;
	int ndevs = 0;
	u64 max_avail;
	u64 dev_offset;

	/*
	 * in the first pass through the devices list, we gather information
	 * about the available holes on each device.
	 */
	list_for_each_entry(device, &fs_devices->alloc_list, dev_alloc_list) {
		if (!test_bit(BTRFS_DEV_STATE_WRITEABLE, &device->dev_state)) {
			WARN(1, KERN_ERR
			       "BTRFS: read-only device in alloc_list\n");
			continue;
		}

		if (!test_bit(BTRFS_DEV_STATE_IN_FS_METADATA,
					&device->dev_state) ||
		    test_bit(BTRFS_DEV_STATE_REPLACE_TGT, &device->dev_state))
			continue;

		if (device->total_bytes > device->bytes_used)
			total_avail = device->total_bytes - device->bytes_used;
		else
			total_avail = 0;

		/* If there is no space on this device, skip it. */
		if (total_avail < ctl->dev_extent_min)
			continue;

		ret = find_free_dev_extent(device, dev_extent_want, &dev_offset,
					   &max_avail);
		if (ret && ret != -ENOSPC)
			return ret;

		if (ret == 0)
			max_avail = dev_extent_want;

		if (max_avail < ctl->dev_extent_min) {
			if (btrfs_test_opt(info, ENOSPC_DEBUG))
				btrfs_debug(info,
			"%s: devid %llu has no free space, have=%llu want=%llu",
					    __func__, device->devid, max_avail,
					    ctl->dev_extent_min);
			continue;
		}

		if (ndevs == fs_devices->rw_devices) {
			WARN(1, "%s: found more than %llu devices\n",
			     __func__, fs_devices->rw_devices);
			break;
		}
		devices_info[ndevs].dev_offset = dev_offset;
		devices_info[ndevs].max_avail = max_avail;
		devices_info[ndevs].total_avail = total_avail;
		devices_info[ndevs].dev = device;
		++ndevs;
	}
	ctl->ndevs = ndevs;

	/*
	 * now sort the devices by hole size / available space
	 */
	sort(devices_info, ndevs, sizeof(struct btrfs_device_info),
	     btrfs_cmp_device_info, NULL);

	return 0;
}