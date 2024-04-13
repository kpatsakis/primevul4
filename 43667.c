static void direct_pte_prefetch(struct kvm_vcpu *vcpu, u64 *sptep)
{
	struct kvm_mmu_page *sp;

	/*
	 * Since it's no accessed bit on EPT, it's no way to
	 * distinguish between actually accessed translations
	 * and prefetched, so disable pte prefetch if EPT is
	 * enabled.
	 */
	if (!shadow_accessed_mask)
		return;

	sp = page_header(__pa(sptep));
	if (sp->role.level > PT_PAGE_TABLE_LEVEL)
		return;

	__direct_pte_prefetch(vcpu, sp, sptep);
}
