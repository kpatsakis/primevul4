static void nonpaging_new_cr3(struct kvm_vcpu *vcpu)
{
	mmu_free_roots(vcpu);
}
