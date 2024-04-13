_xfs_buf_read(
	xfs_buf_t		*bp,
	xfs_buf_flags_t		flags)
{
	ASSERT(!(flags & XBF_WRITE));
	ASSERT(bp->b_maps[0].bm_bn != XFS_BUF_DADDR_NULL);

	bp->b_flags &= ~(XBF_WRITE | XBF_ASYNC | XBF_READ_AHEAD);
	bp->b_flags |= flags & (XBF_READ | XBF_ASYNC | XBF_READ_AHEAD);

	xfs_buf_iorequest(bp);
	if (flags & XBF_ASYNC)
		return 0;
	return xfs_buf_iowait(bp);
}
