__vma_address(struct page *page, struct vm_area_struct *vma)
{
	pgoff_t pgoff = page->index << (PAGE_CACHE_SHIFT - PAGE_SHIFT);

	if (unlikely(is_vm_hugetlb_page(vma)))
		pgoff = page->index << huge_page_order(page_hstate(page));

	return vma->vm_start + ((pgoff - vma->vm_pgoff) << PAGE_SHIFT);
}
