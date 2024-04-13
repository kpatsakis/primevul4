xfs_file_dio_aio_write(
	struct kiocb		*iocb,
	struct iov_iter		*from)
{
	struct file		*file = iocb->ki_filp;
	struct address_space	*mapping = file->f_mapping;
	struct inode		*inode = mapping->host;
	struct xfs_inode	*ip = XFS_I(inode);
	struct xfs_mount	*mp = ip->i_mount;
	ssize_t			ret = 0;
	int			unaligned_io = 0;
	int			iolock;
	size_t			count = iov_iter_count(from);
	loff_t			pos = iocb->ki_pos;
	struct xfs_buftarg	*target = XFS_IS_REALTIME_INODE(ip) ?
					mp->m_rtdev_targp : mp->m_ddev_targp;

	/* DIO must be aligned to device logical sector size */
	if ((pos | count) & target->bt_logical_sectormask)
		return -XFS_ERROR(EINVAL);

	/* "unaligned" here means not aligned to a filesystem block */
	if ((pos & mp->m_blockmask) || ((pos + count) & mp->m_blockmask))
		unaligned_io = 1;

	/*
	 * We don't need to take an exclusive lock unless there page cache needs
	 * to be invalidated or unaligned IO is being executed. We don't need to
	 * consider the EOF extension case here because
	 * xfs_file_aio_write_checks() will relock the inode as necessary for
	 * EOF zeroing cases and fill out the new inode size as appropriate.
	 */
	if (unaligned_io || mapping->nrpages)
		iolock = XFS_IOLOCK_EXCL;
	else
		iolock = XFS_IOLOCK_SHARED;
	xfs_rw_ilock(ip, iolock);

	/*
	 * Recheck if there are cached pages that need invalidate after we got
	 * the iolock to protect against other threads adding new pages while
	 * we were waiting for the iolock.
	 */
	if (mapping->nrpages && iolock == XFS_IOLOCK_SHARED) {
		xfs_rw_iunlock(ip, iolock);
		iolock = XFS_IOLOCK_EXCL;
		xfs_rw_ilock(ip, iolock);
	}

	ret = xfs_file_aio_write_checks(file, &pos, &count, &iolock);
	if (ret)
		goto out;
	iov_iter_truncate(from, count);

	if (mapping->nrpages) {
		ret = filemap_write_and_wait_range(VFS_I(ip)->i_mapping,
						    pos, -1);
		if (ret)
			goto out;
		truncate_pagecache_range(VFS_I(ip), pos, -1);
	}

	/*
	 * If we are doing unaligned IO, wait for all other IO to drain,
	 * otherwise demote the lock if we had to flush cached pages
	 */
	if (unaligned_io)
		inode_dio_wait(inode);
	else if (iolock == XFS_IOLOCK_EXCL) {
		xfs_rw_ilock_demote(ip, XFS_IOLOCK_EXCL);
		iolock = XFS_IOLOCK_SHARED;
	}

	trace_xfs_file_direct_write(ip, count, iocb->ki_pos, 0);
	ret = generic_file_direct_write(iocb, from, pos);

out:
	xfs_rw_iunlock(ip, iolock);

	/* No fallback to buffered IO on errors for XFS. */
	ASSERT(ret < 0 || ret == count);
	return ret;
}
