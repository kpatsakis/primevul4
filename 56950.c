static inline int get_free_page(struct xen_blkif_ring *ring, struct page **page)
{
	unsigned long flags;

	spin_lock_irqsave(&ring->free_pages_lock, flags);
	if (list_empty(&ring->free_pages)) {
		BUG_ON(ring->free_pages_num != 0);
		spin_unlock_irqrestore(&ring->free_pages_lock, flags);
		return gnttab_alloc_pages(1, page);
	}
	BUG_ON(ring->free_pages_num == 0);
	page[0] = list_first_entry(&ring->free_pages, struct page, lru);
	list_del(&page[0]->lru);
	ring->free_pages_num--;
	spin_unlock_irqrestore(&ring->free_pages_lock, flags);

	return 0;
}
