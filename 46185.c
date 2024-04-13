void btrfs_run_delayed_iputs(struct btrfs_root *root)
{
	LIST_HEAD(list);
	struct btrfs_fs_info *fs_info = root->fs_info;
	struct delayed_iput *delayed;
	int empty;

	spin_lock(&fs_info->delayed_iput_lock);
	empty = list_empty(&fs_info->delayed_iputs);
	spin_unlock(&fs_info->delayed_iput_lock);
	if (empty)
		return;

	down_read(&fs_info->delayed_iput_sem);

	spin_lock(&fs_info->delayed_iput_lock);
	list_splice_init(&fs_info->delayed_iputs, &list);
	spin_unlock(&fs_info->delayed_iput_lock);

	while (!list_empty(&list)) {
		delayed = list_entry(list.next, struct delayed_iput, list);
		list_del(&delayed->list);
		iput(delayed->inode);
		kfree(delayed);
	}

	up_read(&root->fs_info->delayed_iput_sem);
}
