static int ubifs_set_page_dirty(struct page *page)
{
	int ret;

	ret = __set_page_dirty_nobuffers(page);
	/*
	 * An attempt to dirty a page without budgeting for it - should not
	 * happen.
	 */
	ubifs_assert(ret == 0);
	return ret;
}
