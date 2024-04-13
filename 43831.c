spte_write_protect(struct kvm *kvm, u64 *sptep, bool *flush, bool pt_protect)
{
	u64 spte = *sptep;

	if (!is_writable_pte(spte) &&
	      !(pt_protect && spte_is_locklessly_modifiable(spte)))
		return false;

	rmap_printk("rmap_write_protect: spte %p %llx\n", sptep, *sptep);

	if (__drop_large_spte(kvm, sptep)) {
		*flush |= true;
		return true;
	}

	if (pt_protect)
		spte &= ~SPTE_MMU_WRITEABLE;
	spte = spte & ~PT_WRITABLE_MASK;

	*flush |= mmu_spte_update(sptep, spte);
	return false;
}
