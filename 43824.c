static bool set_mmio_spte(struct kvm *kvm, u64 *sptep, gfn_t gfn,
			  pfn_t pfn, unsigned access)
{
	if (unlikely(is_noslot_pfn(pfn))) {
		mark_mmio_spte(kvm, sptep, gfn, access);
		return true;
	}

	return false;
}
