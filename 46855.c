static int handle_xrstors(struct kvm_vcpu *vcpu)
{
	skip_emulated_instruction(vcpu);
	WARN(1, "this should never happen\n");
	return 1;
}
