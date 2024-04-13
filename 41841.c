xfs_bwrite(
	struct xfs_buf		*bp)
{
	int			error;

	ASSERT(xfs_buf_islocked(bp));

	bp->b_flags |= XBF_WRITE;
	bp->b_flags &= ~(XBF_ASYNC | XBF_READ | _XBF_DELWRI_Q);

	xfs_bdstrat_cb(bp);

	error = xfs_buf_iowait(bp);
	if (error) {
		xfs_force_shutdown(bp->b_target->bt_mount,
				   SHUTDOWN_META_IO_ERROR);
	}
	return error;
}
