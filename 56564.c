static inline void __vmx_flush_tlb(struct kvm_vcpu *vcpu, int vpid)
{
	if (enable_ept) {
		if (!VALID_PAGE(vcpu->arch.mmu.root_hpa))
			return;
		ept_sync_context(construct_eptp(vcpu, vcpu->arch.mmu.root_hpa));
	} else {
		vpid_sync_context(vpid);
	}
}
