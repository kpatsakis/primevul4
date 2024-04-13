static void mark_mmio_spte(struct kvm *kvm, u64 *sptep, u64 gfn,
			   unsigned access)
{
	unsigned int gen = kvm_current_mmio_generation(kvm);
	u64 mask = generation_mmio_spte_mask(gen);

	access &= ACC_WRITE_MASK | ACC_USER_MASK;
	mask |= shadow_mmio_mask | access | gfn << PAGE_SHIFT;

	trace_mark_mmio_spte(sptep, gfn, access, gen);
	mmu_spte_set(sptep, mask);
}
