static void ept_save_pdptrs(struct kvm_vcpu *vcpu)
{
	if (is_paging(vcpu) && is_pae(vcpu) && !is_long_mode(vcpu)) {
		vcpu->arch.mmu.pdptrs[0] = vmcs_read64(GUEST_PDPTR0);
		vcpu->arch.mmu.pdptrs[1] = vmcs_read64(GUEST_PDPTR1);
		vcpu->arch.mmu.pdptrs[2] = vmcs_read64(GUEST_PDPTR2);
		vcpu->arch.mmu.pdptrs[3] = vmcs_read64(GUEST_PDPTR3);
	}

	__set_bit(VCPU_EXREG_PDPTR,
		  (unsigned long *)&vcpu->arch.regs_avail);
	__set_bit(VCPU_EXREG_PDPTR,
		  (unsigned long *)&vcpu->arch.regs_dirty);
}
