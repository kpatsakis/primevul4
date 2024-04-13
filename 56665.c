static void vmx_set_cr3(struct kvm_vcpu *vcpu, unsigned long cr3)
{
	unsigned long guest_cr3;
	u64 eptp;

	guest_cr3 = cr3;
	if (enable_ept) {
		eptp = construct_eptp(vcpu, cr3);
		vmcs_write64(EPT_POINTER, eptp);
		if (is_paging(vcpu) || is_guest_mode(vcpu))
			guest_cr3 = kvm_read_cr3(vcpu);
		else
			guest_cr3 = vcpu->kvm->arch.ept_identity_map_addr;
		ept_load_pdptrs(vcpu);
	}

	vmx_flush_tlb(vcpu);
	vmcs_writel(GUEST_CR3, guest_cr3);
}
