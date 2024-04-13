xfs_bioerror_relse(
	struct xfs_buf	*bp)
{
	int64_t		fl = bp->b_flags;
	/*
	 * No need to wait until the buffer is unpinned.
	 * We aren't flushing it.
	 *
	 * chunkhold expects B_DONE to be set, whether
	 * we actually finish the I/O or not. We don't want to
	 * change that interface.
	 */
	XFS_BUF_UNREAD(bp);
	XFS_BUF_DONE(bp);
	xfs_buf_stale(bp);
	bp->b_iodone = NULL;
	if (!(fl & XBF_ASYNC)) {
		/*
		 * Mark b_error and B_ERROR _both_.
		 * Lot's of chunkcache code assumes that.
		 * There's no reason to mark error for
		 * ASYNC buffers.
		 */
		xfs_buf_ioerror(bp, EIO);
		complete(&bp->b_iowait);
	} else {
		xfs_buf_relse(bp);
	}

	return EIO;
}
