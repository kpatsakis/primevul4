static int read_user_stack_slow(void __user *ptr, void *ret, int nb)
{
	pgd_t *pgdir;
	pte_t *ptep, pte;
	unsigned shift;
	unsigned long addr = (unsigned long) ptr;
	unsigned long offset;
	unsigned long pfn;
	void *kaddr;

	pgdir = current->mm->pgd;
	if (!pgdir)
		return -EFAULT;

	ptep = find_linux_pte_or_hugepte(pgdir, addr, &shift);
	if (!shift)
		shift = PAGE_SHIFT;

	/* align address to page boundary */
	offset = addr & ((1UL << shift) - 1);
	addr -= offset;

	if (ptep == NULL)
		return -EFAULT;
	pte = *ptep;
	if (!pte_present(pte) || !(pte_val(pte) & _PAGE_USER))
		return -EFAULT;
	pfn = pte_pfn(pte);
	if (!page_is_ram(pfn))
		return -EFAULT;

	/* no highmem to worry about here */
	kaddr = pfn_to_kaddr(pfn);
	memcpy(ret, kaddr + offset, nb);
	return 0;
}
