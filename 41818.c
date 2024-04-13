xfs_buf_iodone_work(
	struct work_struct	*work)
{
	struct xfs_buf		*bp =
		container_of(work, xfs_buf_t, b_iodone_work);
	bool			read = !!(bp->b_flags & XBF_READ);

	bp->b_flags &= ~(XBF_READ | XBF_WRITE | XBF_READ_AHEAD);
	if (read && bp->b_ops)
		bp->b_ops->verify_read(bp);

	if (bp->b_iodone)
		(*(bp->b_iodone))(bp);
	else if (bp->b_flags & XBF_ASYNC)
		xfs_buf_relse(bp);
	else {
		ASSERT(read && bp->b_ops);
		complete(&bp->b_iowait);
	}
}
