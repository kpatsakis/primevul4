static void kvm_mmu_commit_zap_page(struct kvm *kvm,
				    struct list_head *invalid_list)
{
	struct kvm_mmu_page *sp, *nsp;

	if (list_empty(invalid_list))
		return;

	/*
	 * wmb: make sure everyone sees our modifications to the page tables
	 * rmb: make sure we see changes to vcpu->mode
	 */
	smp_mb();

	/*
	 * Wait for all vcpus to exit guest mode and/or lockless shadow
	 * page table walks.
	 */
	kvm_flush_remote_tlbs(kvm);

	list_for_each_entry_safe(sp, nsp, invalid_list, link) {
		WARN_ON(!sp->role.invalid || sp->root_count);
		kvm_mmu_free_page(sp);
	}
}
