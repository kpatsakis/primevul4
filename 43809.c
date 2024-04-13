static bool prepare_zap_oldest_mmu_page(struct kvm *kvm,
					struct list_head *invalid_list)
{
	struct kvm_mmu_page *sp;

	if (list_empty(&kvm->arch.active_mmu_pages))
		return false;

	sp = list_entry(kvm->arch.active_mmu_pages.prev,
			struct kvm_mmu_page, link);
	kvm_mmu_prepare_zap_page(kvm, sp, invalid_list);

	return true;
}
