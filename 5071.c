static void reset_balance_state(struct btrfs_fs_info *fs_info)
{
	struct btrfs_balance_control *bctl = fs_info->balance_ctl;
	int ret;

	BUG_ON(!fs_info->balance_ctl);

	spin_lock(&fs_info->balance_lock);
	fs_info->balance_ctl = NULL;
	spin_unlock(&fs_info->balance_lock);

	kfree(bctl);
	ret = del_balance_item(fs_info);
	if (ret)
		btrfs_handle_fs_error(fs_info, ret, NULL);
}