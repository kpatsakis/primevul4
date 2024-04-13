static void drop_spte(struct kvm *kvm, u64 *sptep)
{
	if (mmu_spte_clear_track_bits(sptep))
		rmap_remove(kvm, sptep);
}
