static u64 walk_shadow_page_get_mmio_spte(struct kvm_vcpu *vcpu, u64 addr)
{
	struct kvm_shadow_walk_iterator iterator;
	u64 spte = 0ull;

	walk_shadow_page_lockless_begin(vcpu);
	for_each_shadow_entry_lockless(vcpu, addr, iterator, spte)
		if (!is_shadow_present_pte(spte))
			break;
	walk_shadow_page_lockless_end(vcpu);

	return spte;
}
