mem_to_page(
	void			*addr)
{
	if ((!is_vmalloc_addr(addr))) {
		return virt_to_page(addr);
	} else {
		return vmalloc_to_page(addr);
	}
}
