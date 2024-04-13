void kvm_before_handle_nmi(struct kvm_vcpu *vcpu)
{
	percpu_write(current_vcpu, vcpu);
}
