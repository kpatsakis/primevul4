static int handle_xsaves(struct kvm_vcpu *vcpu)
{
	skip_emulated_instruction(vcpu);
	WARN(1, "this should never happen\n");
	return 1;
}
