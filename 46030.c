static int emulator_get_cpl(struct kvm_vcpu *vcpu)
{
	return kvm_x86_ops->get_cpl(vcpu);
}
