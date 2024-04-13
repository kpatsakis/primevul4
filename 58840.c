static void *__netdev_alloc_frag(unsigned int fragsz, gfp_t gfp_mask)
{
	struct page_frag_cache *nc;
	unsigned long flags;
	void *data;

	local_irq_save(flags);
	nc = this_cpu_ptr(&netdev_alloc_cache);
	data = page_frag_alloc(nc, fragsz, gfp_mask);
	local_irq_restore(flags);
	return data;
}
