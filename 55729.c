static int numamigrate_isolate_page(pg_data_t *pgdat, struct page *page)
{
	int page_lru;

	VM_BUG_ON_PAGE(compound_order(page) && !PageTransHuge(page), page);

	/* Avoid migrating to a node that is nearly full */
	if (!migrate_balanced_pgdat(pgdat, 1UL << compound_order(page)))
		return 0;

	if (isolate_lru_page(page))
		return 0;

	/*
	 * migrate_misplaced_transhuge_page() skips page migration's usual
	 * check on page_count(), so we must do it here, now that the page
	 * has been isolated: a GUP pin, or any other pin, prevents migration.
	 * The expected page count is 3: 1 for page's mapcount and 1 for the
	 * caller's pin and 1 for the reference taken by isolate_lru_page().
	 */
	if (PageTransHuge(page) && page_count(page) != 3) {
		putback_lru_page(page);
		return 0;
	}

	page_lru = page_is_file_cache(page);
	mod_node_page_state(page_pgdat(page), NR_ISOLATED_ANON + page_lru,
				hpage_nr_pages(page));

	/*
	 * Isolating the page has taken another reference, so the
	 * caller's reference can be safely dropped without the page
	 * disappearing underneath us during migration.
	 */
	put_page(page);
	return 1;
}
