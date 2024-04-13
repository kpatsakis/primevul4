static void xen_blk_drain_io(struct xen_blkif_ring *ring)
{
	struct xen_blkif *blkif = ring->blkif;

	atomic_set(&blkif->drain, 1);
	do {
		if (atomic_read(&ring->inflight) == 0)
			break;
		wait_for_completion_interruptible_timeout(
				&blkif->drain_complete, HZ);

		if (!atomic_read(&blkif->drain))
			break;
	} while (!kthread_should_stop());
	atomic_set(&blkif->drain, 0);
}
