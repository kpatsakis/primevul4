void kvm_mmu_slot_remove_write_access(struct kvm *kvm, int slot)
{
	struct kvm_memory_slot *memslot;
	gfn_t last_gfn;
	int i;

	memslot = id_to_memslot(kvm->memslots, slot);
	last_gfn = memslot->base_gfn + memslot->npages - 1;

	spin_lock(&kvm->mmu_lock);

	for (i = PT_PAGE_TABLE_LEVEL;
	     i < PT_PAGE_TABLE_LEVEL + KVM_NR_PAGE_SIZES; ++i) {
		unsigned long *rmapp;
		unsigned long last_index, index;

		rmapp = memslot->arch.rmap[i - PT_PAGE_TABLE_LEVEL];
		last_index = gfn_to_index(last_gfn, memslot->base_gfn, i);

		for (index = 0; index <= last_index; ++index, ++rmapp) {
			if (*rmapp)
				__rmap_write_protect(kvm, rmapp, false);

			if (need_resched() || spin_needbreak(&kvm->mmu_lock)) {
				kvm_flush_remote_tlbs(kvm);
				cond_resched_lock(&kvm->mmu_lock);
			}
		}
	}

	kvm_flush_remote_tlbs(kvm);
	spin_unlock(&kvm->mmu_lock);
}
