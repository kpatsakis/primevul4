static int ubifs_releasepage(struct page *page, gfp_t unused_gfp_flags)
{
	/*
	 * An attempt to release a dirty page without budgeting for it - should
	 * not happen.
	 */
	if (PageWriteback(page))
		return 0;
	ubifs_assert(PagePrivate(page));
	ubifs_assert(0);
	ClearPagePrivate(page);
	ClearPageChecked(page);
	return 1;
}
