static bool remove_migration_pte(struct page *page, struct vm_area_struct *vma,
				 unsigned long addr, void *old)
{
	struct page_vma_mapped_walk pvmw = {
		.page = old,
		.vma = vma,
		.address = addr,
		.flags = PVMW_SYNC | PVMW_MIGRATION,
	};
	struct page *new;
	pte_t pte;
	swp_entry_t entry;

	VM_BUG_ON_PAGE(PageTail(page), page);
	while (page_vma_mapped_walk(&pvmw)) {
		if (PageKsm(page))
			new = page;
		else
			new = page - pvmw.page->index +
				linear_page_index(vma, pvmw.address);

		get_page(new);
		pte = pte_mkold(mk_pte(new, READ_ONCE(vma->vm_page_prot)));
		if (pte_swp_soft_dirty(*pvmw.pte))
			pte = pte_mksoft_dirty(pte);

		/*
		 * Recheck VMA as permissions can change since migration started
		 */
		entry = pte_to_swp_entry(*pvmw.pte);
		if (is_write_migration_entry(entry))
			pte = maybe_mkwrite(pte, vma);

		flush_dcache_page(new);
#ifdef CONFIG_HUGETLB_PAGE
		if (PageHuge(new)) {
			pte = pte_mkhuge(pte);
			pte = arch_make_huge_pte(pte, vma, new, 0);
			set_huge_pte_at(vma->vm_mm, pvmw.address, pvmw.pte, pte);
			if (PageAnon(new))
				hugepage_add_anon_rmap(new, vma, pvmw.address);
			else
				page_dup_rmap(new, true);
		} else
#endif
		{
			set_pte_at(vma->vm_mm, pvmw.address, pvmw.pte, pte);

			if (PageAnon(new))
				page_add_anon_rmap(new, vma, pvmw.address, false);
			else
				page_add_file_rmap(new, false);
		}
		if (vma->vm_flags & VM_LOCKED && !PageTransCompound(new))
			mlock_vma_page(new);

		/* No need to invalidate - it was non-present before */
		update_mmu_cache(vma, pvmw.address, pvmw.pte);
	}

	return true;
}
