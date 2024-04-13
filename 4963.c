void btrfs_rm_dev_replace_free_srcdev(struct btrfs_device *srcdev)
{
	struct btrfs_fs_devices *fs_devices = srcdev->fs_devices;

	mutex_lock(&uuid_mutex);

	btrfs_close_bdev(srcdev);
	synchronize_rcu();
	btrfs_free_device(srcdev);

	/* if this is no devs we rather delete the fs_devices */
	if (!fs_devices->num_devices) {
		/*
		 * On a mounted FS, num_devices can't be zero unless it's a
		 * seed. In case of a seed device being replaced, the replace
		 * target added to the sprout FS, so there will be no more
		 * device left under the seed FS.
		 */
		ASSERT(fs_devices->seeding);

		list_del_init(&fs_devices->seed_list);
		close_fs_devices(fs_devices);
		free_fs_devices(fs_devices);
	}
	mutex_unlock(&uuid_mutex);
}