xfs_buf_ioend(
	struct xfs_buf	*bp,
	int		schedule)
{
	bool		read = !!(bp->b_flags & XBF_READ);

	trace_xfs_buf_iodone(bp, _RET_IP_);

	if (bp->b_error == 0)
		bp->b_flags |= XBF_DONE;

	if (bp->b_iodone || (read && bp->b_ops) || (bp->b_flags & XBF_ASYNC)) {
		if (schedule) {
			INIT_WORK(&bp->b_iodone_work, xfs_buf_iodone_work);
			queue_work(xfslogd_workqueue, &bp->b_iodone_work);
		} else {
			xfs_buf_iodone_work(&bp->b_iodone_work);
		}
	} else {
		bp->b_flags &= ~(XBF_READ | XBF_WRITE | XBF_READ_AHEAD);
		complete(&bp->b_iowait);
	}
}
