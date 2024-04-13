xfs_bioerror(
	xfs_buf_t *bp)
{
#ifdef XFSERRORDEBUG
	ASSERT(XFS_BUF_ISREAD(bp) || bp->b_iodone);
#endif

	/*
	 * No need to wait until the buffer is unpinned, we aren't flushing it.
	 */
	xfs_buf_ioerror(bp, EIO);

	/*
	 * We're calling xfs_buf_ioend, so delete XBF_DONE flag.
	 */
	XFS_BUF_UNREAD(bp);
	XFS_BUF_UNDONE(bp);
	xfs_buf_stale(bp);

	xfs_buf_ioend(bp, 0);

	return EIO;
}
