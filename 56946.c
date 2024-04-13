static void blkif_notify_work(struct xen_blkif_ring *ring)
{
	ring->waiting_reqs = 1;
	wake_up(&ring->wq);
}
