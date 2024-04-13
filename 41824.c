xfs_buf_iowait(
	xfs_buf_t		*bp)
{
	trace_xfs_buf_iowait(bp, _RET_IP_);

	if (!bp->b_error)
		wait_for_completion(&bp->b_iowait);

	trace_xfs_buf_iowait_done(bp, _RET_IP_);
	return bp->b_error;
}
