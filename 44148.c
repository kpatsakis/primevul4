int rmap_walk(struct page *page, struct rmap_walk_control *rwc)
{
	if (unlikely(PageKsm(page)))
		return rmap_walk_ksm(page, rwc);
	else if (PageAnon(page))
		return rmap_walk_anon(page, rwc);
	else
		return rmap_walk_file(page, rwc);
}
