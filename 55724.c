int migrate_misplaced_page(struct page *page, struct vm_area_struct *vma,
			   int node)
{
	pg_data_t *pgdat = NODE_DATA(node);
	int isolated;
	int nr_remaining;
	LIST_HEAD(migratepages);

	/*
	 * Don't migrate file pages that are mapped in multiple processes
	 * with execute permissions as they are probably shared libraries.
	 */
	if (page_mapcount(page) != 1 && page_is_file_cache(page) &&
	    (vma->vm_flags & VM_EXEC))
		goto out;

	/*
	 * Rate-limit the amount of data that is being migrated to a node.
	 * Optimal placement is no good if the memory bus is saturated and
	 * all the time is being spent migrating!
	 */
	if (numamigrate_update_ratelimit(pgdat, 1))
		goto out;

	isolated = numamigrate_isolate_page(pgdat, page);
	if (!isolated)
		goto out;

	list_add(&page->lru, &migratepages);
	nr_remaining = migrate_pages(&migratepages, alloc_misplaced_dst_page,
				     NULL, node, MIGRATE_ASYNC,
				     MR_NUMA_MISPLACED);
	if (nr_remaining) {
		if (!list_empty(&migratepages)) {
			list_del(&page->lru);
			dec_node_page_state(page, NR_ISOLATED_ANON +
					page_is_file_cache(page));
			putback_lru_page(page);
		}
		isolated = 0;
	} else
		count_vm_numa_event(NUMA_PAGE_MIGRATE);
	BUG_ON(!list_empty(&migratepages));
	return isolated;

out:
	put_page(page);
	return 0;
}
