int handle_mmio_page_fault_common(struct kvm_vcpu *vcpu, u64 addr, bool direct)
{
	u64 spte;

	if (quickly_check_mmio_pf(vcpu, addr, direct))
		return RET_MMIO_PF_EMULATE;

	spte = walk_shadow_page_get_mmio_spte(vcpu, addr);

	if (is_mmio_spte(spte)) {
		gfn_t gfn = get_mmio_spte_gfn(spte);
		unsigned access = get_mmio_spte_access(spte);

		if (!check_mmio_spte(vcpu->kvm, spte))
			return RET_MMIO_PF_INVALID;

		if (direct)
			addr = 0;

		trace_handle_mmio_page_fault(addr, gfn, access);
		vcpu_cache_mmio_info(vcpu, addr, gfn, access);
		return RET_MMIO_PF_EMULATE;
	}

	/*
	 * It's ok if the gva is remapped by other cpus on shadow guest,
	 * it's a BUG if the gfn is not a mmio page.
	 */
	if (direct && !check_direct_spte_mmio_pf(spte))
		return RET_MMIO_PF_BUG;

	/*
	 * If the page table is zapped by other cpus, let CPU fault again on
	 * the address.
	 */
	return RET_MMIO_PF_RETRY;
}
