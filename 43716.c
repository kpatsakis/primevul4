int kvm_mmu_create(struct kvm_vcpu *vcpu)
{
	ASSERT(vcpu);

	vcpu->arch.walk_mmu = &vcpu->arch.mmu;
	vcpu->arch.mmu.root_hpa = INVALID_PAGE;
	vcpu->arch.mmu.translate_gpa = translate_gpa;
	vcpu->arch.nested_mmu.translate_gpa = translate_nested_gpa;

	return alloc_mmu_pages(vcpu);
}
