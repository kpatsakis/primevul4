int migrate_pages(struct list_head *from, new_page_t get_new_page,
		free_page_t put_new_page, unsigned long private,
		enum migrate_mode mode, int reason)
{
	int retry = 1;
	int nr_failed = 0;
	int nr_succeeded = 0;
	int pass = 0;
	struct page *page;
	struct page *page2;
	int swapwrite = current->flags & PF_SWAPWRITE;
	int rc;

	if (!swapwrite)
		current->flags |= PF_SWAPWRITE;

	for(pass = 0; pass < 10 && retry; pass++) {
		retry = 0;

		list_for_each_entry_safe(page, page2, from, lru) {
			cond_resched();

			if (PageHuge(page))
				rc = unmap_and_move_huge_page(get_new_page,
						put_new_page, private, page,
						pass > 2, mode, reason);
			else
				rc = unmap_and_move(get_new_page, put_new_page,
						private, page, pass > 2, mode,
						reason);

			switch(rc) {
			case -ENOMEM:
				nr_failed++;
				goto out;
			case -EAGAIN:
				retry++;
				break;
			case MIGRATEPAGE_SUCCESS:
				nr_succeeded++;
				break;
			default:
				/*
				 * Permanent failure (-EBUSY, -ENOSYS, etc.):
				 * unlike -EAGAIN case, the failed page is
				 * removed from migration page list and not
				 * retried in the next outer loop.
				 */
				nr_failed++;
				break;
			}
		}
	}
	nr_failed += retry;
	rc = nr_failed;
out:
	if (nr_succeeded)
		count_vm_events(PGMIGRATE_SUCCESS, nr_succeeded);
	if (nr_failed)
		count_vm_events(PGMIGRATE_FAIL, nr_failed);
	trace_mm_migrate_pages(nr_succeeded, nr_failed, mode, reason);

	if (!swapwrite)
		current->flags &= ~PF_SWAPWRITE;

	return rc;
}
