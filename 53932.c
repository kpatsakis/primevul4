int kvm_emulate_halt(struct kvm_vcpu *vcpu)
{
	kvm_x86_ops->skip_emulated_instruction(vcpu);
	return kvm_vcpu_halt(vcpu);
}
