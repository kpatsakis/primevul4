static bool sync_mmio_spte(struct kvm *kvm, u64 *sptep, gfn_t gfn,
			   unsigned access, int *nr_present)
{
	if (unlikely(is_mmio_spte(*sptep))) {
		if (gfn != get_mmio_spte_gfn(*sptep)) {
			mmu_spte_clear_no_track(sptep);
			return true;
		}

		(*nr_present)++;
		mark_mmio_spte(kvm, sptep, gfn, access);
		return true;
	}

	return false;
}
