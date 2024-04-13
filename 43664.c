static void destroy_kvm_mmu(struct kvm_vcpu *vcpu)
{
	ASSERT(vcpu);
	if (VALID_PAGE(vcpu->arch.mmu.root_hpa))
		/* mmu.free() should set root_hpa = INVALID_PAGE */
		vcpu->arch.mmu.free(vcpu);
}
