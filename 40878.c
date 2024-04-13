static void blkif_notify_work(struct xen_blkif *blkif)
{
	blkif->waiting_reqs = 1;
	wake_up(&blkif->wq);
}
