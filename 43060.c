int kvm_set_cr3(struct kvm_vcpu *vcpu, unsigned long cr3)
{
	if (cr3 == kvm_read_cr3(vcpu) && !pdptrs_changed(vcpu)) {
		kvm_mmu_sync_roots(vcpu);
		kvm_mmu_flush_tlb(vcpu);
		return 0;
	}

	if (is_long_mode(vcpu)) {
		if (cr3 & CR3_L_MODE_RESERVED_BITS)
			return 1;
	} else if (is_pae(vcpu) && is_paging(vcpu) &&
		   !load_pdptrs(vcpu, vcpu->arch.walk_mmu, cr3))
		return 1;

	vcpu->arch.cr3 = cr3;
	__set_bit(VCPU_EXREG_CR3, (ulong *)&vcpu->arch.regs_avail);
	kvm_mmu_new_cr3(vcpu);
	return 0;
}
