xfs_buf_hold(
	xfs_buf_t		*bp)
{
	trace_xfs_buf_hold(bp, _RET_IP_);
	atomic_inc(&bp->b_hold);
}
