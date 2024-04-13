static pfn_t pte_prefetch_gfn_to_pfn(struct kvm_vcpu *vcpu, gfn_t gfn,
				     bool no_dirty_log)
{
	struct kvm_memory_slot *slot;

	slot = gfn_to_memslot_dirty_bitmap(vcpu, gfn, no_dirty_log);
	if (!slot)
		return KVM_PFN_ERR_FAULT;

	return gfn_to_pfn_memslot_atomic(slot, gfn);
}
