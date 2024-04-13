void kvm_arch_start_assignment(struct kvm *kvm)
{
	atomic_inc(&kvm->arch.assigned_device_count);
}
