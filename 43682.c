static unsigned long *gfn_to_rmap(struct kvm *kvm, gfn_t gfn, int level)
{
	struct kvm_memory_slot *slot;

	slot = gfn_to_memslot(kvm, gfn);
	return __gfn_to_rmap(gfn, level, slot);
}
