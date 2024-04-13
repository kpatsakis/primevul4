static int kvm_handle_hva(struct kvm *kvm, unsigned long hva,
			  unsigned long data,
			  int (*handler)(struct kvm *kvm, unsigned long *rmapp,
					 struct kvm_memory_slot *slot,
					 unsigned long data))
{
	return kvm_handle_hva_range(kvm, hva, hva + 1, data, handler);
}
