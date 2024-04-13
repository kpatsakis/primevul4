static int balance_kthread(void *data)
{
	struct btrfs_fs_info *fs_info = data;
	int ret = 0;

	mutex_lock(&fs_info->balance_mutex);
	if (fs_info->balance_ctl)
		ret = btrfs_balance(fs_info, fs_info->balance_ctl, NULL);
	mutex_unlock(&fs_info->balance_mutex);

	return ret;
}