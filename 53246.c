static inline void clear_soft_dirty(struct vm_area_struct *vma,
		unsigned long addr, pte_t *pte)
{
	/*
	 * The soft-dirty tracker uses #PF-s to catch writes
	 * to pages, so write-protect the pte as well. See the
	 * Documentation/vm/soft-dirty.txt for full description
	 * of how soft-dirty works.
	 */
	pte_t ptent = *pte;

	if (pte_present(ptent)) {
		ptent = pte_wrprotect(ptent);
		ptent = pte_clear_flags(ptent, _PAGE_SOFT_DIRTY);
	} else if (is_swap_pte(ptent)) {
		ptent = pte_swp_clear_soft_dirty(ptent);
	}

	set_pte_at(vma->vm_mm, addr, pte, ptent);
}
