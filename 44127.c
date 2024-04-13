void hugepage_add_new_anon_rmap(struct page *page,
			struct vm_area_struct *vma, unsigned long address)
{
	BUG_ON(address < vma->vm_start || address >= vma->vm_end);
	atomic_set(&page->_mapcount, 0);
	__hugepage_set_anon_rmap(page, vma, address, 1);
}
