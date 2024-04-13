int nfs_migrate_page(struct address_space *mapping, struct page *newpage,
		struct page *page, enum migrate_mode mode)
{
	/*
	 * If PagePrivate is set, then the page is currently associated with
	 * an in-progress read or write request. Don't try to migrate it.
	 *
	 * FIXME: we could do this in principle, but we'll need a way to ensure
	 *        that we can safely release the inode reference while holding
	 *        the page lock.
	 */
	if (PagePrivate(page))
		return -EBUSY;

	if (!nfs_fscache_release_page(page, GFP_KERNEL))
		return -EBUSY;

	return migrate_page(mapping, newpage, page, mode);
}
