static void free_req(struct xen_blkif *blkif, struct pending_req *req)
{
	unsigned long flags;
	int was_empty;

	spin_lock_irqsave(&blkif->pending_free_lock, flags);
	was_empty = list_empty(&blkif->pending_free);
	list_add(&req->free_list, &blkif->pending_free);
	spin_unlock_irqrestore(&blkif->pending_free_lock, flags);
	if (was_empty)
		wake_up(&blkif->pending_free_wq);
}
