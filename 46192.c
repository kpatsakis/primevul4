int btrfs_start_delalloc_roots(struct btrfs_fs_info *fs_info, int delay_iput,
			       int nr)
{
	struct btrfs_root *root;
	struct list_head splice;
	int ret;

	if (test_bit(BTRFS_FS_STATE_ERROR, &fs_info->fs_state))
		return -EROFS;

	INIT_LIST_HEAD(&splice);

	mutex_lock(&fs_info->delalloc_root_mutex);
	spin_lock(&fs_info->delalloc_root_lock);
	list_splice_init(&fs_info->delalloc_roots, &splice);
	while (!list_empty(&splice) && nr) {
		root = list_first_entry(&splice, struct btrfs_root,
					delalloc_root);
		root = btrfs_grab_fs_root(root);
		BUG_ON(!root);
		list_move_tail(&root->delalloc_root,
			       &fs_info->delalloc_roots);
		spin_unlock(&fs_info->delalloc_root_lock);

		ret = __start_delalloc_inodes(root, delay_iput, nr);
		btrfs_put_fs_root(root);
		if (ret < 0)
			goto out;

		if (nr != -1) {
			nr -= ret;
			WARN_ON(nr < 0);
		}
		spin_lock(&fs_info->delalloc_root_lock);
	}
	spin_unlock(&fs_info->delalloc_root_lock);

	ret = 0;
	atomic_inc(&fs_info->async_submit_draining);
	while (atomic_read(&fs_info->nr_async_submits) ||
	      atomic_read(&fs_info->async_delalloc_pages)) {
		wait_event(fs_info->async_submit_wait,
		   (atomic_read(&fs_info->nr_async_submits) == 0 &&
		    atomic_read(&fs_info->async_delalloc_pages) == 0));
	}
	atomic_dec(&fs_info->async_submit_draining);
out:
	if (!list_empty_careful(&splice)) {
		spin_lock(&fs_info->delalloc_root_lock);
		list_splice_tail(&splice, &fs_info->delalloc_roots);
		spin_unlock(&fs_info->delalloc_root_lock);
	}
	mutex_unlock(&fs_info->delalloc_root_mutex);
	return ret;
}
