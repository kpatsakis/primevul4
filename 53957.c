static void kvm_set_hflags(struct kvm_vcpu *vcpu, unsigned emul_flags)
{
	unsigned changed = vcpu->arch.hflags ^ emul_flags;

	vcpu->arch.hflags = emul_flags;

	if (changed & HF_SMM_MASK)
		kvm_smm_changed(vcpu);
}
