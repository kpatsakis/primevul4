static int pte_list_add(struct kvm_vcpu *vcpu, u64 *spte,
			unsigned long *pte_list)
{
	struct pte_list_desc *desc;
	int i, count = 0;

	if (!*pte_list) {
		rmap_printk("pte_list_add: %p %llx 0->1\n", spte, *spte);
		*pte_list = (unsigned long)spte;
	} else if (!(*pte_list & 1)) {
		rmap_printk("pte_list_add: %p %llx 1->many\n", spte, *spte);
		desc = mmu_alloc_pte_list_desc(vcpu);
		desc->sptes[0] = (u64 *)*pte_list;
		desc->sptes[1] = spte;
		*pte_list = (unsigned long)desc | 1;
		++count;
	} else {
		rmap_printk("pte_list_add: %p %llx many->many\n", spte, *spte);
		desc = (struct pte_list_desc *)(*pte_list & ~1ul);
		while (desc->sptes[PTE_LIST_EXT-1] && desc->more) {
			desc = desc->more;
			count += PTE_LIST_EXT;
		}
		if (desc->sptes[PTE_LIST_EXT-1]) {
			desc->more = mmu_alloc_pte_list_desc(vcpu);
			desc = desc->more;
		}
		for (i = 0; desc->sptes[i]; ++i)
			++count;
		desc->sptes[i] = spte;
	}
	return count;
}
