static void kvm_sync_pages(struct kvm_vcpu *vcpu,  gfn_t gfn)
{
	struct kvm_mmu_page *s;
	LIST_HEAD(invalid_list);
	bool flush = false;

	for_each_gfn_indirect_valid_sp(vcpu->kvm, s, gfn) {
		if (!s->unsync)
			continue;

		WARN_ON(s->role.level != PT_PAGE_TABLE_LEVEL);
		kvm_unlink_unsync_page(vcpu->kvm, s);
		if ((s->role.cr4_pae != !!is_pae(vcpu)) ||
			(vcpu->arch.mmu.sync_page(vcpu, s))) {
			kvm_mmu_prepare_zap_page(vcpu->kvm, s, &invalid_list);
			continue;
		}
		flush = true;
	}

	kvm_mmu_commit_zap_page(vcpu->kvm, &invalid_list);
	if (flush)
		kvm_mmu_flush_tlb(vcpu);
}
