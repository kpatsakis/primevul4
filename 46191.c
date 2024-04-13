int btrfs_start_delalloc_inodes(struct btrfs_root *root, int delay_iput)
{
	int ret;

	if (test_bit(BTRFS_FS_STATE_ERROR, &root->fs_info->fs_state))
		return -EROFS;

	ret = __start_delalloc_inodes(root, delay_iput, -1);
	if (ret > 0)
		ret = 0;
	/*
	 * the filemap_flush will queue IO into the worker threads, but
	 * we have to make sure the IO is actually started and that
	 * ordered extents get created before we return
	 */
	atomic_inc(&root->fs_info->async_submit_draining);
	while (atomic_read(&root->fs_info->nr_async_submits) ||
	      atomic_read(&root->fs_info->async_delalloc_pages)) {
		wait_event(root->fs_info->async_submit_wait,
		   (atomic_read(&root->fs_info->nr_async_submits) == 0 &&
		    atomic_read(&root->fs_info->async_delalloc_pages) == 0));
	}
	atomic_dec(&root->fs_info->async_submit_draining);
	return ret;
}
