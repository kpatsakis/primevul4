int kvm_mmu_get_spte_hierarchy(struct kvm_vcpu *vcpu, u64 addr, u64 sptes[4])
{
	struct kvm_shadow_walk_iterator iterator;
	u64 spte;
	int nr_sptes = 0;

	walk_shadow_page_lockless_begin(vcpu);
	for_each_shadow_entry_lockless(vcpu, addr, iterator, spte) {
		sptes[iterator.level-1] = spte;
		nr_sptes++;
		if (!is_shadow_present_pte(spte))
			break;
	}
	walk_shadow_page_lockless_end(vcpu);

	return nr_sptes;
}
