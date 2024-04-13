gfn_to_memslot_dirty_bitmap(struct kvm_vcpu *vcpu, gfn_t gfn,
			    bool no_dirty_log)
{
	struct kvm_memory_slot *slot;

	slot = gfn_to_memslot(vcpu->kvm, gfn);
	if (!slot || slot->flags & KVM_MEMSLOT_INVALID ||
	      (no_dirty_log && slot->dirty_bitmap))
		slot = NULL;

	return slot;
}
