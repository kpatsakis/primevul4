xfs_buf_wait_unpin(
	xfs_buf_t		*bp)
{
	DECLARE_WAITQUEUE	(wait, current);

	if (atomic_read(&bp->b_pin_count) == 0)
		return;

	add_wait_queue(&bp->b_waiters, &wait);
	for (;;) {
		set_current_state(TASK_UNINTERRUPTIBLE);
		if (atomic_read(&bp->b_pin_count) == 0)
			break;
		io_schedule();
	}
	remove_wait_queue(&bp->b_waiters, &wait);
	set_current_state(TASK_RUNNING);
}
