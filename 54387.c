static int kvm_deassign_ioeventfd(struct kvm *kvm, struct kvm_ioeventfd *args)
{
	enum kvm_bus bus_idx = ioeventfd_bus_from_flags(args->flags);
	int ret = kvm_deassign_ioeventfd_idx(kvm, bus_idx, args);

	if (!args->len && bus_idx == KVM_MMIO_BUS)
		kvm_deassign_ioeventfd_idx(kvm, KVM_FAST_MMIO_BUS, args);

	return ret;
}
