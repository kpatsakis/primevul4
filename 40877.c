static struct pending_req *alloc_req(struct xen_blkif *blkif)
{
	struct pending_req *req = NULL;
	unsigned long flags;

	spin_lock_irqsave(&blkif->pending_free_lock, flags);
	if (!list_empty(&blkif->pending_free)) {
		req = list_entry(blkif->pending_free.next, struct pending_req,
				 free_list);
		list_del(&req->free_list);
	}
	spin_unlock_irqrestore(&blkif->pending_free_lock, flags);
	return req;
}
