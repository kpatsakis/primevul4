static struct btrfs_fs_devices *find_fsid_reverted_metadata(
				struct btrfs_super_block *disk_super)
{
	struct btrfs_fs_devices *fs_devices;

	/*
	 * Handle the case where the scanned device is part of an fs whose last
	 * metadata UUID change reverted it to the original FSID. At the same
	 * time * fs_devices was first created by another constitutent device
	 * which didn't fully observe the operation. This results in an
	 * btrfs_fs_devices created with metadata/fsid different AND
	 * btrfs_fs_devices::fsid_change set AND the metadata_uuid of the
	 * fs_devices equal to the FSID of the disk.
	 */
	list_for_each_entry(fs_devices, &fs_uuids, fs_list) {
		if (memcmp(fs_devices->fsid, fs_devices->metadata_uuid,
			   BTRFS_FSID_SIZE) != 0 &&
		    memcmp(fs_devices->metadata_uuid, disk_super->fsid,
			   BTRFS_FSID_SIZE) == 0 &&
		    fs_devices->fsid_change)
			return fs_devices;
	}

	return NULL;
}