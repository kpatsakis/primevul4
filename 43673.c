fast_pf_fix_direct_spte(struct kvm_vcpu *vcpu, u64 *sptep, u64 spte)
{
	struct kvm_mmu_page *sp = page_header(__pa(sptep));
	gfn_t gfn;

	WARN_ON(!sp->role.direct);

	/*
	 * The gfn of direct spte is stable since it is calculated
	 * by sp->gfn.
	 */
	gfn = kvm_mmu_page_get_gfn(sp, sptep - sp->spt);

	if (cmpxchg64(sptep, spte, spte | PT_WRITABLE_MASK) == spte)
		mark_page_dirty(vcpu->kvm, gfn);

	return true;
}
