static void btrfs_run_delalloc_work(struct btrfs_work *work)
{
	struct btrfs_delalloc_work *delalloc_work;
	struct inode *inode;

	delalloc_work = container_of(work, struct btrfs_delalloc_work,
				     work);
	inode = delalloc_work->inode;
	if (delalloc_work->wait) {
		btrfs_wait_ordered_range(inode, 0, (u64)-1);
	} else {
		filemap_flush(inode->i_mapping);
		if (test_bit(BTRFS_INODE_HAS_ASYNC_EXTENT,
			     &BTRFS_I(inode)->runtime_flags))
			filemap_flush(inode->i_mapping);
	}

	if (delalloc_work->delay_iput)
		btrfs_add_delayed_iput(inode);
	else
		iput(inode);
	complete(&delalloc_work->completion);
}
