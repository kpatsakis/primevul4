static u64 btrfs_num_devices(struct btrfs_fs_info *fs_info)
{
	u64 num_devices = fs_info->fs_devices->num_devices;

	down_read(&fs_info->dev_replace.rwsem);
	if (btrfs_dev_replace_is_ongoing(&fs_info->dev_replace)) {
		ASSERT(num_devices > 1);
		num_devices--;
	}
	up_read(&fs_info->dev_replace.rwsem);

	return num_devices;
}