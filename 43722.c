void kvm_mmu_invalidate_zap_all_pages(struct kvm *kvm)
{
	spin_lock(&kvm->mmu_lock);
	trace_kvm_mmu_invalidate_zap_all_pages(kvm);
	kvm->arch.mmu_valid_gen++;

	/*
	 * Notify all vcpus to reload its shadow page table
	 * and flush TLB. Then all vcpus will switch to new
	 * shadow page table with the new mmu_valid_gen.
	 *
	 * Note: we should do this under the protection of
	 * mmu-lock, otherwise, vcpu would purge shadow page
	 * but miss tlb flush.
	 */
	kvm_reload_remote_mmus(kvm);

	kvm_zap_obsolete_pages(kvm);
	spin_unlock(&kvm->mmu_lock);
}
