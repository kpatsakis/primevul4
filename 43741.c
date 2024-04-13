void kvm_mmu_unload(struct kvm_vcpu *vcpu)
{
	mmu_free_roots(vcpu);
}
