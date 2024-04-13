xfs_buf_trylock(
	struct xfs_buf		*bp)
{
	int			locked;

	locked = down_trylock(&bp->b_sema) == 0;
	if (locked)
		XB_SET_OWNER(bp);
	else if (atomic_read(&bp->b_pin_count) && (bp->b_flags & XBF_STALE))
		xfs_log_force(bp->b_target->bt_mount, 0);

	trace_xfs_buf_trylock(bp, _RET_IP_);
	return locked;
}
