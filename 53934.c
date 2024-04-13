int kvm_emulate_wbinvd(struct kvm_vcpu *vcpu)
{
	kvm_x86_ops->skip_emulated_instruction(vcpu);
	return kvm_emulate_wbinvd_noskip(vcpu);
}
