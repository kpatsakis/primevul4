int xen_blkif_schedule(void *arg)
{
	struct xen_blkif_ring *ring = arg;
	struct xen_blkif *blkif = ring->blkif;
	struct xen_vbd *vbd = &blkif->vbd;
	unsigned long timeout;
	int ret;

	set_freezable();
	while (!kthread_should_stop()) {
		if (try_to_freeze())
			continue;
		if (unlikely(vbd->size != vbd_sz(vbd)))
			xen_vbd_resize(blkif);

		timeout = msecs_to_jiffies(LRU_INTERVAL);

		timeout = wait_event_interruptible_timeout(
			ring->wq,
			ring->waiting_reqs || kthread_should_stop(),
			timeout);
		if (timeout == 0)
			goto purge_gnt_list;
		timeout = wait_event_interruptible_timeout(
			ring->pending_free_wq,
			!list_empty(&ring->pending_free) ||
			kthread_should_stop(),
			timeout);
		if (timeout == 0)
			goto purge_gnt_list;

		ring->waiting_reqs = 0;
		smp_mb(); /* clear flag *before* checking for work */

		ret = do_block_io_op(ring);
		if (ret > 0)
			ring->waiting_reqs = 1;
		if (ret == -EACCES)
			wait_event_interruptible(ring->shutdown_wq,
						 kthread_should_stop());

purge_gnt_list:
		if (blkif->vbd.feature_gnt_persistent &&
		    time_after(jiffies, ring->next_lru)) {
			purge_persistent_gnt(ring);
			ring->next_lru = jiffies + msecs_to_jiffies(LRU_INTERVAL);
		}

		/* Shrink if we have more than xen_blkif_max_buffer_pages */
		shrink_free_pagepool(ring, xen_blkif_max_buffer_pages);

		if (log_stats && time_after(jiffies, ring->st_print))
			print_stats(ring);
	}

	/* Drain pending purge work */
	flush_work(&ring->persistent_purge_work);

	if (log_stats)
		print_stats(ring);

	ring->xenblkd = NULL;

	return 0;
}
