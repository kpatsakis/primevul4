static struct btrfs_fs_devices *find_fsid_with_metadata_uuid(
				struct btrfs_super_block *disk_super)
{

	struct btrfs_fs_devices *fs_devices;

	/*
	 * Handle scanned device having completed its fsid change but
	 * belonging to a fs_devices that was created by first scanning
	 * a device which didn't have its fsid/metadata_uuid changed
	 * at all and the CHANGING_FSID_V2 flag set.
	 */
	list_for_each_entry(fs_devices, &fs_uuids, fs_list) {
		if (fs_devices->fsid_change &&
		    memcmp(disk_super->metadata_uuid, fs_devices->fsid,
			   BTRFS_FSID_SIZE) == 0 &&
		    memcmp(fs_devices->fsid, fs_devices->metadata_uuid,
			   BTRFS_FSID_SIZE) == 0) {
			return fs_devices;
		}
	}
	/*
	 * Handle scanned device having completed its fsid change but
	 * belonging to a fs_devices that was created by a device that
	 * has an outdated pair of fsid/metadata_uuid and
	 * CHANGING_FSID_V2 flag set.
	 */
	list_for_each_entry(fs_devices, &fs_uuids, fs_list) {
		if (fs_devices->fsid_change &&
		    memcmp(fs_devices->metadata_uuid,
			   fs_devices->fsid, BTRFS_FSID_SIZE) != 0 &&
		    memcmp(disk_super->metadata_uuid, fs_devices->metadata_uuid,
			   BTRFS_FSID_SIZE) == 0) {
			return fs_devices;
		}
	}

	return find_fsid(disk_super->fsid, disk_super->metadata_uuid);
}