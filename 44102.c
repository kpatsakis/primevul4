static void __putback_lru_fast(struct pagevec *pvec, int pgrescued)
{
	count_vm_events(UNEVICTABLE_PGMUNLOCKED, pagevec_count(pvec));
	/*
	 *__pagevec_lru_add() calls release_pages() so we don't call
	 * put_page() explicitly
	 */
	__pagevec_lru_add(pvec);
	count_vm_events(UNEVICTABLE_PGRESCUED, pgrescued);
}
