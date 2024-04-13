int kvm_mmu_setup(struct kvm_vcpu *vcpu)
{
	ASSERT(vcpu);
	ASSERT(!VALID_PAGE(vcpu->arch.mmu.root_hpa));

	return init_kvm_mmu(vcpu);
}
