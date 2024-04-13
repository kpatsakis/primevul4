static ssize_t btrfs_direct_IO(struct kiocb *iocb, struct iov_iter *iter,
			       loff_t offset)
{
	struct file *file = iocb->ki_filp;
	struct inode *inode = file->f_mapping->host;
	struct btrfs_root *root = BTRFS_I(inode)->root;
	struct btrfs_dio_data dio_data = { 0 };
	size_t count = 0;
	int flags = 0;
	bool wakeup = true;
	bool relock = false;
	ssize_t ret;

	if (check_direct_IO(BTRFS_I(inode)->root, iocb, iter, offset))
		return 0;

	inode_dio_begin(inode);
	smp_mb__after_atomic();

	/*
	 * The generic stuff only does filemap_write_and_wait_range, which
	 * isn't enough if we've written compressed pages to this area, so
	 * we need to flush the dirty pages again to make absolutely sure
	 * that any outstanding dirty pages are on disk.
	 */
	count = iov_iter_count(iter);
	if (test_bit(BTRFS_INODE_HAS_ASYNC_EXTENT,
		     &BTRFS_I(inode)->runtime_flags))
		filemap_fdatawrite_range(inode->i_mapping, offset,
					 offset + count - 1);

	if (iov_iter_rw(iter) == WRITE) {
		/*
		 * If the write DIO is beyond the EOF, we need update
		 * the isize, but it is protected by i_mutex. So we can
		 * not unlock the i_mutex at this case.
		 */
		if (offset + count <= inode->i_size) {
			mutex_unlock(&inode->i_mutex);
			relock = true;
		}
		ret = btrfs_delalloc_reserve_space(inode, count);
		if (ret)
			goto out;
		dio_data.outstanding_extents = div64_u64(count +
						BTRFS_MAX_EXTENT_SIZE - 1,
						BTRFS_MAX_EXTENT_SIZE);

		/*
		 * We need to know how many extents we reserved so that we can
		 * do the accounting properly if we go over the number we
		 * originally calculated.  Abuse current->journal_info for this.
		 */
		dio_data.reserve = round_up(count, root->sectorsize);
		current->journal_info = &dio_data;
	} else if (test_bit(BTRFS_INODE_READDIO_NEED_LOCK,
				     &BTRFS_I(inode)->runtime_flags)) {
		inode_dio_end(inode);
		flags = DIO_LOCKING | DIO_SKIP_HOLES;
		wakeup = false;
	}

	ret = __blockdev_direct_IO(iocb, inode,
				   BTRFS_I(inode)->root->fs_info->fs_devices->latest_bdev,
				   iter, offset, btrfs_get_blocks_direct, NULL,
				   btrfs_submit_direct, flags);
	if (iov_iter_rw(iter) == WRITE) {
		current->journal_info = NULL;
		if (ret < 0 && ret != -EIOCBQUEUED) {
			if (dio_data.reserve)
				btrfs_delalloc_release_space(inode,
							dio_data.reserve);
		} else if (ret >= 0 && (size_t)ret < count)
			btrfs_delalloc_release_space(inode,
						     count - (size_t)ret);
	}
out:
	if (wakeup)
		inode_dio_end(inode);
	if (relock)
		mutex_lock(&inode->i_mutex);

	return ret;
}
