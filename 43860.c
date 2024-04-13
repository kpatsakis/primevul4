static int handle_pause(struct kvm_vcpu *vcpu)
{
	skip_emulated_instruction(vcpu);
	kvm_vcpu_on_spin(vcpu);

	return 1;
}
