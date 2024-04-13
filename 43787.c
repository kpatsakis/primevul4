static void mmu_sync_children(struct kvm_vcpu *vcpu,
			      struct kvm_mmu_page *parent)
{
	int i;
	struct kvm_mmu_page *sp;
	struct mmu_page_path parents;
	struct kvm_mmu_pages pages;
	LIST_HEAD(invalid_list);

	kvm_mmu_pages_init(parent, &parents, &pages);
	while (mmu_unsync_walk(parent, &pages)) {
		bool protected = false;

		for_each_sp(pages, sp, parents, i)
			protected |= rmap_write_protect(vcpu->kvm, sp->gfn);

		if (protected)
			kvm_flush_remote_tlbs(vcpu->kvm);

		for_each_sp(pages, sp, parents, i) {
			kvm_sync_page(vcpu, sp, &invalid_list);
			mmu_pages_clear_parents(&parents);
		}
		kvm_mmu_commit_zap_page(vcpu->kvm, &invalid_list);
		cond_resched_lock(&vcpu->kvm->mmu_lock);
		kvm_mmu_pages_init(parent, &parents, &pages);
	}
}
