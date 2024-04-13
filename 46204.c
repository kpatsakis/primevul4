static int btrfs_writepage_end_io_hook(struct page *page, u64 start, u64 end,
				struct extent_state *state, int uptodate)
{
	struct inode *inode = page->mapping->host;
	struct btrfs_root *root = BTRFS_I(inode)->root;
	struct btrfs_ordered_extent *ordered_extent = NULL;
	struct btrfs_workqueue *wq;
	btrfs_work_func_t func;

	trace_btrfs_writepage_end_io_hook(page, start, end, uptodate);

	ClearPagePrivate2(page);
	if (!btrfs_dec_test_ordered_pending(inode, &ordered_extent, start,
					    end - start + 1, uptodate))
		return 0;

	if (btrfs_is_free_space_inode(inode)) {
		wq = root->fs_info->endio_freespace_worker;
		func = btrfs_freespace_write_helper;
	} else {
		wq = root->fs_info->endio_write_workers;
		func = btrfs_endio_write_helper;
	}

	btrfs_init_work(&ordered_extent->work, func, finish_ordered_fn, NULL,
			NULL);
	btrfs_queue_work(wq, &ordered_extent->work);

	return 0;
}
