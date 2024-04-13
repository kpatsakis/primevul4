static int __kvm_sync_page(struct kvm_vcpu *vcpu, struct kvm_mmu_page *sp,
			   struct list_head *invalid_list, bool clear_unsync)
{
	if (sp->role.cr4_pae != !!is_pae(vcpu)) {
		kvm_mmu_prepare_zap_page(vcpu->kvm, sp, invalid_list);
		return 1;
	}

	if (clear_unsync)
		kvm_unlink_unsync_page(vcpu->kvm, sp);

	if (vcpu->arch.mmu.sync_page(vcpu, sp)) {
		kvm_mmu_prepare_zap_page(vcpu->kvm, sp, invalid_list);
		return 1;
	}

	kvm_mmu_flush_tlb(vcpu);
	return 0;
}
