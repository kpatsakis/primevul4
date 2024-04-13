static inline int get_free_page(struct xen_blkif *blkif, struct page **page)
{
	unsigned long flags;

	spin_lock_irqsave(&blkif->free_pages_lock, flags);
	if (list_empty(&blkif->free_pages)) {
		BUG_ON(blkif->free_pages_num != 0);
		spin_unlock_irqrestore(&blkif->free_pages_lock, flags);
		return alloc_xenballooned_pages(1, page, false);
	}
	BUG_ON(blkif->free_pages_num == 0);
	page[0] = list_first_entry(&blkif->free_pages, struct page, lru);
	list_del(&page[0]->lru);
	blkif->free_pages_num--;
	spin_unlock_irqrestore(&blkif->free_pages_lock, flags);

	return 0;
}
