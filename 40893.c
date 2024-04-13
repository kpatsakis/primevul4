static void xen_blk_drain_io(struct xen_blkif *blkif)
{
	atomic_set(&blkif->drain, 1);
	do {
		/* The initial value is one, and one refcnt taken at the
		 * start of the xen_blkif_schedule thread. */
		if (atomic_read(&blkif->refcnt) <= 2)
			break;
		wait_for_completion_interruptible_timeout(
				&blkif->drain_complete, HZ);

		if (!atomic_read(&blkif->drain))
			break;
	} while (!kthread_should_stop());
	atomic_set(&blkif->drain, 0);
}
