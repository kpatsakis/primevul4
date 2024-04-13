static void kvm_update_dr6(struct kvm_vcpu *vcpu)
{
	if (!(vcpu->guest_debug & KVM_GUESTDBG_USE_HW_BP))
		kvm_x86_ops->set_dr6(vcpu, vcpu->arch.dr6);
}
