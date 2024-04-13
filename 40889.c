static inline void put_free_pages(struct xen_blkif *blkif, struct page **page,
                                  int num)
{
	unsigned long flags;
	int i;

	spin_lock_irqsave(&blkif->free_pages_lock, flags);
	for (i = 0; i < num; i++)
		list_add(&page[i]->lru, &blkif->free_pages);
	blkif->free_pages_num += num;
	spin_unlock_irqrestore(&blkif->free_pages_lock, flags);
}
