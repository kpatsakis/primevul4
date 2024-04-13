static void mmu_pte_write_flush_tlb(struct kvm_vcpu *vcpu, bool zap_page,
				    bool remote_flush, bool local_flush)
{
	if (zap_page)
		return;

	if (remote_flush)
		kvm_flush_remote_tlbs(vcpu->kvm);
	else if (local_flush)
		kvm_mmu_flush_tlb(vcpu);
}
