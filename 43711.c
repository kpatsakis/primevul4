static struct kvm_mmu_page *kvm_mmu_alloc_page(struct kvm_vcpu *vcpu,
					       u64 *parent_pte, int direct)
{
	struct kvm_mmu_page *sp;

	sp = mmu_memory_cache_alloc(&vcpu->arch.mmu_page_header_cache);
	sp->spt = mmu_memory_cache_alloc(&vcpu->arch.mmu_page_cache);
	if (!direct)
		sp->gfns = mmu_memory_cache_alloc(&vcpu->arch.mmu_page_cache);
	set_page_private(virt_to_page(sp->spt), (unsigned long)sp);

	/*
	 * The active_mmu_pages list is the FIFO list, do not move the
	 * page until it is zapped. kvm_zap_obsolete_pages depends on
	 * this feature. See the comments in kvm_zap_obsolete_pages().
	 */
	list_add(&sp->link, &vcpu->kvm->arch.active_mmu_pages);
	sp->parent_ptes = 0;
	mmu_page_add_parent_pte(vcpu, sp, parent_pte);
	kvm_mod_used_mmu_pages(vcpu->kvm, +1);
	return sp;
}
