int kvm_init_shadow_mmu(struct kvm_vcpu *vcpu, struct kvm_mmu *context)
{
	int r;
	bool smep = kvm_read_cr4_bits(vcpu, X86_CR4_SMEP);
	ASSERT(vcpu);
	ASSERT(!VALID_PAGE(vcpu->arch.mmu.root_hpa));

	if (!is_paging(vcpu))
		r = nonpaging_init_context(vcpu, context);
	else if (is_long_mode(vcpu))
		r = paging64_init_context(vcpu, context);
	else if (is_pae(vcpu))
		r = paging32E_init_context(vcpu, context);
	else
		r = paging32_init_context(vcpu, context);

	vcpu->arch.mmu.base_role.nxe = is_nx(vcpu);
	vcpu->arch.mmu.base_role.cr4_pae = !!is_pae(vcpu);
	vcpu->arch.mmu.base_role.cr0_wp  = is_write_protection(vcpu);
	vcpu->arch.mmu.base_role.smep_andnot_wp
		= smep && !is_write_protection(vcpu);

	return r;
}
