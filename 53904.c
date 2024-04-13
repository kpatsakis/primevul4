void kvm_arch_end_assignment(struct kvm *kvm)
{
	atomic_dec(&kvm->arch.assigned_device_count);
}
