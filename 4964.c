static struct btrfs_fs_devices *find_fsid_changed(
					struct btrfs_super_block *disk_super)
{
	struct btrfs_fs_devices *fs_devices;

	/*
	 * Handles the case where scanned device is part of an fs that had
	 * multiple successful changes of FSID but currently device didn't
	 * observe it. Meaning our fsid will be different than theirs. We need
	 * to handle two subcases :
	 *  1 - The fs still continues to have different METADATA/FSID uuids.
	 *  2 - The fs is switched back to its original FSID (METADATA/FSID
	 *  are equal).
	 */
	list_for_each_entry(fs_devices, &fs_uuids, fs_list) {
		/* Changed UUIDs */
		if (memcmp(fs_devices->metadata_uuid, fs_devices->fsid,
			   BTRFS_FSID_SIZE) != 0 &&
		    memcmp(fs_devices->metadata_uuid, disk_super->metadata_uuid,
			   BTRFS_FSID_SIZE) == 0 &&
		    memcmp(fs_devices->fsid, disk_super->fsid,
			   BTRFS_FSID_SIZE) != 0)
			return fs_devices;

		/* Unchanged UUIDs */
		if (memcmp(fs_devices->metadata_uuid, fs_devices->fsid,
			   BTRFS_FSID_SIZE) == 0 &&
		    memcmp(fs_devices->fsid, disk_super->metadata_uuid,
			   BTRFS_FSID_SIZE) == 0)
			return fs_devices;
	}

	return NULL;
}