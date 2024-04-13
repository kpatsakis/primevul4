void page_move_anon_rmap(struct page *page,
	struct vm_area_struct *vma, unsigned long address)
{
	struct anon_vma *anon_vma = vma->anon_vma;

	VM_BUG_ON_PAGE(!PageLocked(page), page);
	VM_BUG_ON(!anon_vma);
	VM_BUG_ON_PAGE(page->index != linear_page_index(vma, address), page);

	anon_vma = (void *) anon_vma + PAGE_MAPPING_ANON;
	page->mapping = (struct address_space *) anon_vma;
}
