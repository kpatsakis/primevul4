int kvm_arch_create_memslot(struct kvm *kvm, struct kvm_memory_slot *slot,
			    unsigned long npages)
{
	return kvmppc_core_create_memslot(kvm, slot, npages);
}
