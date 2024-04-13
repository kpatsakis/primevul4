xfs_file_write_iter(
	struct kiocb		*iocb,
	struct iov_iter		*from)
{
	struct file		*file = iocb->ki_filp;
	struct address_space	*mapping = file->f_mapping;
	struct inode		*inode = mapping->host;
	struct xfs_inode	*ip = XFS_I(inode);
	ssize_t			ret;
	size_t			ocount = iov_iter_count(from);

	XFS_STATS_INC(xs_write_calls);

	if (ocount == 0)
		return 0;

	if (XFS_FORCED_SHUTDOWN(ip->i_mount))
		return -EIO;

	if (unlikely(file->f_flags & O_DIRECT))
		ret = xfs_file_dio_aio_write(iocb, from);
	else
		ret = xfs_file_buffered_aio_write(iocb, from);

	if (ret > 0) {
		ssize_t err;

		XFS_STATS_ADD(xs_write_bytes, ret);

		/* Handle various SYNC-type writes */
		err = generic_write_sync(file, iocb->ki_pos - ret, ret);
		if (err < 0)
			ret = err;
	}
	return ret;
}
