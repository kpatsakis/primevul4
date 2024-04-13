static int init_kvm_mmu(struct kvm_vcpu *vcpu)
{
	if (mmu_is_nested(vcpu))
		return init_kvm_nested_mmu(vcpu);
	else if (tdp_enabled)
		return init_kvm_tdp_mmu(vcpu);
	else
		return init_kvm_softmmu(vcpu);
}
