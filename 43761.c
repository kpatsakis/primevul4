static int mmu_alloc_roots(struct kvm_vcpu *vcpu)
{
	if (vcpu->arch.mmu.direct_map)
		return mmu_alloc_direct_roots(vcpu);
	else
		return mmu_alloc_shadow_roots(vcpu);
}
