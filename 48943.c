xfs_file_buffered_aio_write(
	struct kiocb		*iocb,
	struct iov_iter		*from)
{
	struct file		*file = iocb->ki_filp;
	struct address_space	*mapping = file->f_mapping;
	struct inode		*inode = mapping->host;
	struct xfs_inode	*ip = XFS_I(inode);
	ssize_t			ret;
	int			enospc = 0;
	int			iolock = XFS_IOLOCK_EXCL;
	loff_t			pos = iocb->ki_pos;
	size_t			count = iov_iter_count(from);

	xfs_rw_ilock(ip, iolock);

	ret = xfs_file_aio_write_checks(file, &pos, &count, &iolock);
	if (ret)
		goto out;

	iov_iter_truncate(from, count);
	/* We can write back this queue in page reclaim */
	current->backing_dev_info = mapping->backing_dev_info;

write_retry:
	trace_xfs_file_buffered_write(ip, count, iocb->ki_pos, 0);
	ret = generic_perform_write(file, from, pos);
	if (likely(ret >= 0))
		iocb->ki_pos = pos + ret;
	/*
	 * If we just got an ENOSPC, try to write back all dirty inodes to
	 * convert delalloc space to free up some of the excess reserved
	 * metadata space.
	 */
	if (ret == -ENOSPC && !enospc) {
		enospc = 1;
		xfs_flush_inodes(ip->i_mount);
		goto write_retry;
	}

	current->backing_dev_info = NULL;
out:
	xfs_rw_iunlock(ip, iolock);
	return ret;
}
