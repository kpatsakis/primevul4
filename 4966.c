struct btrfs_device *btrfs_find_device(struct btrfs_fs_devices *fs_devices,
				       u64 devid, u8 *uuid, u8 *fsid)
{
	struct btrfs_device *device;
	struct btrfs_fs_devices *seed_devs;

	if (!fsid || !memcmp(fs_devices->metadata_uuid, fsid, BTRFS_FSID_SIZE)) {
		list_for_each_entry(device, &fs_devices->devices, dev_list) {
			if (device->devid == devid &&
			    (!uuid || memcmp(device->uuid, uuid,
					     BTRFS_UUID_SIZE) == 0))
				return device;
		}
	}

	list_for_each_entry(seed_devs, &fs_devices->seed_list, seed_list) {
		if (!fsid ||
		    !memcmp(seed_devs->metadata_uuid, fsid, BTRFS_FSID_SIZE)) {
			list_for_each_entry(device, &seed_devs->devices,
					    dev_list) {
				if (device->devid == devid &&
				    (!uuid || memcmp(device->uuid, uuid,
						     BTRFS_UUID_SIZE) == 0))
					return device;
			}
		}
	}

	return NULL;
}