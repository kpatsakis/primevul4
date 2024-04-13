static int kvm_mmu_prepare_zap_page(struct kvm *kvm, struct kvm_mmu_page *sp,
				    struct list_head *invalid_list)
{
	int ret;

	trace_kvm_mmu_prepare_zap_page(sp);
	++kvm->stat.mmu_shadow_zapped;
	ret = mmu_zap_unsync_children(kvm, sp, invalid_list);
	kvm_mmu_page_unlink_children(kvm, sp);
	kvm_mmu_unlink_parents(kvm, sp);

	if (!sp->role.invalid && !sp->role.direct)
		unaccount_shadowed(kvm, sp->gfn);

	if (sp->unsync)
		kvm_unlink_unsync_page(kvm, sp);
	if (!sp->root_count) {
		/* Count self */
		ret++;
		list_move(&sp->link, invalid_list);
		kvm_mod_used_mmu_pages(kvm, -1);
	} else {
		list_move(&sp->link, &kvm->arch.active_mmu_pages);

		/*
		 * The obsolete pages can not be used on any vcpus.
		 * See the comments in kvm_mmu_invalidate_zap_all_pages().
		 */
		if (!sp->role.invalid && !is_obsolete_sp(kvm, sp))
			kvm_reload_remote_mmus(kvm);
	}

	sp->role.invalid = 1;
	return ret;
}
