static noinline struct btrfs_device *device_list_add(const char *path,
			   struct btrfs_super_block *disk_super,
			   bool *new_device_added)
{
	struct btrfs_device *device;
	struct btrfs_fs_devices *fs_devices = NULL;
	struct rcu_string *name;
	u64 found_transid = btrfs_super_generation(disk_super);
	u64 devid = btrfs_stack_device_id(&disk_super->dev_item);
	bool has_metadata_uuid = (btrfs_super_incompat_flags(disk_super) &
		BTRFS_FEATURE_INCOMPAT_METADATA_UUID);
	bool fsid_change_in_progress = (btrfs_super_flags(disk_super) &
					BTRFS_SUPER_FLAG_CHANGING_FSID_V2);

	if (fsid_change_in_progress) {
		if (!has_metadata_uuid)
			fs_devices = find_fsid_inprogress(disk_super);
		else
			fs_devices = find_fsid_changed(disk_super);
	} else if (has_metadata_uuid) {
		fs_devices = find_fsid_with_metadata_uuid(disk_super);
	} else {
		fs_devices = find_fsid_reverted_metadata(disk_super);
		if (!fs_devices)
			fs_devices = find_fsid(disk_super->fsid, NULL);
	}


	if (!fs_devices) {
		if (has_metadata_uuid)
			fs_devices = alloc_fs_devices(disk_super->fsid,
						      disk_super->metadata_uuid);
		else
			fs_devices = alloc_fs_devices(disk_super->fsid, NULL);

		if (IS_ERR(fs_devices))
			return ERR_CAST(fs_devices);

		fs_devices->fsid_change = fsid_change_in_progress;

		mutex_lock(&fs_devices->device_list_mutex);
		list_add(&fs_devices->fs_list, &fs_uuids);

		device = NULL;
	} else {
		mutex_lock(&fs_devices->device_list_mutex);
		device = btrfs_find_device(fs_devices, devid,
				disk_super->dev_item.uuid, NULL);

		/*
		 * If this disk has been pulled into an fs devices created by
		 * a device which had the CHANGING_FSID_V2 flag then replace the
		 * metadata_uuid/fsid values of the fs_devices.
		 */
		if (fs_devices->fsid_change &&
		    found_transid > fs_devices->latest_generation) {
			memcpy(fs_devices->fsid, disk_super->fsid,
					BTRFS_FSID_SIZE);

			if (has_metadata_uuid)
				memcpy(fs_devices->metadata_uuid,
				       disk_super->metadata_uuid,
				       BTRFS_FSID_SIZE);
			else
				memcpy(fs_devices->metadata_uuid,
				       disk_super->fsid, BTRFS_FSID_SIZE);

			fs_devices->fsid_change = false;
		}
	}

	if (!device) {
		if (fs_devices->opened) {
			mutex_unlock(&fs_devices->device_list_mutex);
			return ERR_PTR(-EBUSY);
		}

		device = btrfs_alloc_device(NULL, &devid,
					    disk_super->dev_item.uuid);
		if (IS_ERR(device)) {
			mutex_unlock(&fs_devices->device_list_mutex);
			/* we can safely leave the fs_devices entry around */
			return device;
		}

		name = rcu_string_strdup(path, GFP_NOFS);
		if (!name) {
			btrfs_free_device(device);
			mutex_unlock(&fs_devices->device_list_mutex);
			return ERR_PTR(-ENOMEM);
		}
		rcu_assign_pointer(device->name, name);

		list_add_rcu(&device->dev_list, &fs_devices->devices);
		fs_devices->num_devices++;

		device->fs_devices = fs_devices;
		*new_device_added = true;

		if (disk_super->label[0])
			pr_info(
	"BTRFS: device label %s devid %llu transid %llu %s scanned by %s (%d)\n",
				disk_super->label, devid, found_transid, path,
				current->comm, task_pid_nr(current));
		else
			pr_info(
	"BTRFS: device fsid %pU devid %llu transid %llu %s scanned by %s (%d)\n",
				disk_super->fsid, devid, found_transid, path,
				current->comm, task_pid_nr(current));

	} else if (!device->name || strcmp(device->name->str, path)) {
		/*
		 * When FS is already mounted.
		 * 1. If you are here and if the device->name is NULL that
		 *    means this device was missing at time of FS mount.
		 * 2. If you are here and if the device->name is different
		 *    from 'path' that means either
		 *      a. The same device disappeared and reappeared with
		 *         different name. or
		 *      b. The missing-disk-which-was-replaced, has
		 *         reappeared now.
		 *
		 * We must allow 1 and 2a above. But 2b would be a spurious
		 * and unintentional.
		 *
		 * Further in case of 1 and 2a above, the disk at 'path'
		 * would have missed some transaction when it was away and
		 * in case of 2a the stale bdev has to be updated as well.
		 * 2b must not be allowed at all time.
		 */

		/*
		 * For now, we do allow update to btrfs_fs_device through the
		 * btrfs dev scan cli after FS has been mounted.  We're still
		 * tracking a problem where systems fail mount by subvolume id
		 * when we reject replacement on a mounted FS.
		 */
		if (!fs_devices->opened && found_transid < device->generation) {
			/*
			 * That is if the FS is _not_ mounted and if you
			 * are here, that means there is more than one
			 * disk with same uuid and devid.We keep the one
			 * with larger generation number or the last-in if
			 * generation are equal.
			 */
			mutex_unlock(&fs_devices->device_list_mutex);
			return ERR_PTR(-EEXIST);
		}

		/*
		 * We are going to replace the device path for a given devid,
		 * make sure it's the same device if the device is mounted
		 */
		if (device->bdev) {
			int error;
			dev_t path_dev;

			error = lookup_bdev(path, &path_dev);
			if (error) {
				mutex_unlock(&fs_devices->device_list_mutex);
				return ERR_PTR(error);
			}

			if (device->bdev->bd_dev != path_dev) {
				mutex_unlock(&fs_devices->device_list_mutex);
				/*
				 * device->fs_info may not be reliable here, so
				 * pass in a NULL instead. This avoids a
				 * possible use-after-free when the fs_info and
				 * fs_info->sb are already torn down.
				 */
				btrfs_warn_in_rcu(NULL,
	"duplicate device %s devid %llu generation %llu scanned by %s (%d)",
						  path, devid, found_transid,
						  current->comm,
						  task_pid_nr(current));
				return ERR_PTR(-EEXIST);
			}
			btrfs_info_in_rcu(device->fs_info,
	"devid %llu device path %s changed to %s scanned by %s (%d)",
					  devid, rcu_str_deref(device->name),
					  path, current->comm,
					  task_pid_nr(current));
		}

		name = rcu_string_strdup(path, GFP_NOFS);
		if (!name) {
			mutex_unlock(&fs_devices->device_list_mutex);
			return ERR_PTR(-ENOMEM);
		}
		rcu_string_free(device->name);
		rcu_assign_pointer(device->name, name);
		if (test_bit(BTRFS_DEV_STATE_MISSING, &device->dev_state)) {
			fs_devices->missing_devices--;
			clear_bit(BTRFS_DEV_STATE_MISSING, &device->dev_state);
		}
	}

	/*
	 * Unmount does not free the btrfs_device struct but would zero
	 * generation along with most of the other members. So just update
	 * it back. We need it to pick the disk with largest generation
	 * (as above).
	 */
	if (!fs_devices->opened) {
		device->generation = found_transid;
		fs_devices->latest_generation = max_t(u64, found_transid,
						fs_devices->latest_generation);
	}

	fs_devices->total_devices = btrfs_super_num_devices(disk_super);

	mutex_unlock(&fs_devices->device_list_mutex);
	return device;
}