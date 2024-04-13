vma_address(struct page *page, struct vm_area_struct *vma)
{
	unsigned long address = __vma_address(page, vma);

	/* page should be within @vma mapping range */
	VM_BUG_ON(address < vma->vm_start || address >= vma->vm_end);

	return address;
}
