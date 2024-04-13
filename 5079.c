static noinline struct btrfs_fs_devices *find_fsid(
		const u8 *fsid, const u8 *metadata_fsid)
{
	struct btrfs_fs_devices *fs_devices;

	ASSERT(fsid);

	/* Handle non-split brain cases */
	list_for_each_entry(fs_devices, &fs_uuids, fs_list) {
		if (metadata_fsid) {
			if (memcmp(fsid, fs_devices->fsid, BTRFS_FSID_SIZE) == 0
			    && memcmp(metadata_fsid, fs_devices->metadata_uuid,
				      BTRFS_FSID_SIZE) == 0)
				return fs_devices;
		} else {
			if (memcmp(fsid, fs_devices->fsid, BTRFS_FSID_SIZE) == 0)
				return fs_devices;
		}
	}
	return NULL;
}