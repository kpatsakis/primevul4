int kvm_arch_vcpu_runnable(struct kvm_vcpu *vcpu)
{
	if (is_guest_mode(vcpu) && kvm_x86_ops->check_nested_events)
		kvm_x86_ops->check_nested_events(vcpu, false);

	return kvm_vcpu_running(vcpu) || kvm_vcpu_has_events(vcpu);
}
