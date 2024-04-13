int kvm_is_visible_gfn(struct kvm *kvm, gfn_t gfn)
{
	int i;
	struct kvm_memslots *slots = kvm_memslots(kvm);

	for (i = 0; i < KVM_MEMORY_SLOTS; ++i) {
		struct kvm_memory_slot *memslot = &slots->memslots[i];

		if (memslot->flags & KVM_MEMSLOT_INVALID)
			continue;

		if (gfn >= memslot->base_gfn
		    && gfn < memslot->base_gfn + memslot->npages)
			return 1;
	}
	return 0;
}
