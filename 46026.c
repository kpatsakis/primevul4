int emulate_clts(struct kvm_vcpu *vcpu)
{
	kvm_x86_ops->set_cr0(vcpu, kvm_read_cr0_bits(vcpu, ~X86_CR0_TS));
	kvm_x86_ops->fpu_activate(vcpu);
	return X86EMUL_CONTINUE;
}
