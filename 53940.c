int kvm_get_msr(struct kvm_vcpu *vcpu, struct msr_data *msr)
{
	return kvm_x86_ops->get_msr(vcpu, msr);
}
