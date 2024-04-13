void btrfs_close_devices(struct btrfs_fs_devices *fs_devices)
{
	LIST_HEAD(list);
	struct btrfs_fs_devices *tmp;

	mutex_lock(&uuid_mutex);
	close_fs_devices(fs_devices);
	if (!fs_devices->opened)
		list_splice_init(&fs_devices->seed_list, &list);

	list_for_each_entry_safe(fs_devices, tmp, &list, seed_list) {
		close_fs_devices(fs_devices);
		list_del(&fs_devices->seed_list);
		free_fs_devices(fs_devices);
	}
	mutex_unlock(&uuid_mutex);
}