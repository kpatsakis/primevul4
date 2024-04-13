static int handle_halt(struct kvm_vcpu *vcpu)
{
	return kvm_emulate_halt(vcpu);
}
