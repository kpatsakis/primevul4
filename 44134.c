void page_add_anon_rmap(struct page *page,
	struct vm_area_struct *vma, unsigned long address)
{
	do_page_add_anon_rmap(page, vma, address, 0);
}
