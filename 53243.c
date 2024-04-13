static struct page *can_gather_numa_stats(pte_t pte, struct vm_area_struct *vma,
		unsigned long addr)
{
	struct page *page;
	int nid;

	if (!pte_present(pte))
		return NULL;

	page = vm_normal_page(vma, addr, pte);
	if (!page)
		return NULL;

	if (PageReserved(page))
		return NULL;

	nid = page_to_nid(page);
	if (!node_isset(nid, node_states[N_MEMORY]))
		return NULL;

	return page;
}
