static void smaps_account(struct mem_size_stats *mss, struct page *page,
		unsigned long size, bool young, bool dirty)
{
	int mapcount;

	if (PageAnon(page))
		mss->anonymous += size;

	mss->resident += size;
	/* Accumulate the size in pages that have been accessed. */
	if (young || PageReferenced(page))
		mss->referenced += size;
	mapcount = page_mapcount(page);
	if (mapcount >= 2) {
		u64 pss_delta;

		if (dirty || PageDirty(page))
			mss->shared_dirty += size;
		else
			mss->shared_clean += size;
		pss_delta = (u64)size << PSS_SHIFT;
		do_div(pss_delta, mapcount);
		mss->pss += pss_delta;
	} else {
		if (dirty || PageDirty(page))
			mss->private_dirty += size;
		else
			mss->private_clean += size;
		mss->pss += (u64)size << PSS_SHIFT;
	}
}
