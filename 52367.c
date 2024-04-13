void kvm_vcpu_mtrr_init(struct kvm_vcpu *vcpu)
{
	INIT_LIST_HEAD(&vcpu->arch.mtrr_state.head);
}
