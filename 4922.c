void __exit btrfs_cleanup_fs_uuids(void)
{
	struct btrfs_fs_devices *fs_devices;

	while (!list_empty(&fs_uuids)) {
		fs_devices = list_entry(fs_uuids.next,
					struct btrfs_fs_devices, fs_list);
		list_del(&fs_devices->fs_list);
		free_fs_devices(fs_devices);
	}
}