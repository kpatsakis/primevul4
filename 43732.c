void kvm_mmu_pte_write(struct kvm_vcpu *vcpu, gpa_t gpa,
		       const u8 *new, int bytes)
{
	gfn_t gfn = gpa >> PAGE_SHIFT;
	union kvm_mmu_page_role mask = { .word = 0 };
	struct kvm_mmu_page *sp;
	LIST_HEAD(invalid_list);
	u64 entry, gentry, *spte;
	int npte;
	bool remote_flush, local_flush, zap_page;

	/*
	 * If we don't have indirect shadow pages, it means no page is
	 * write-protected, so we can exit simply.
	 */
	if (!ACCESS_ONCE(vcpu->kvm->arch.indirect_shadow_pages))
		return;

	zap_page = remote_flush = local_flush = false;

	pgprintk("%s: gpa %llx bytes %d\n", __func__, gpa, bytes);

	gentry = mmu_pte_write_fetch_gpte(vcpu, &gpa, new, &bytes);

	/*
	 * No need to care whether allocation memory is successful
	 * or not since pte prefetch is skiped if it does not have
	 * enough objects in the cache.
	 */
	mmu_topup_memory_caches(vcpu);

	spin_lock(&vcpu->kvm->mmu_lock);
	++vcpu->kvm->stat.mmu_pte_write;
	kvm_mmu_audit(vcpu, AUDIT_PRE_PTE_WRITE);

	mask.cr0_wp = mask.cr4_pae = mask.nxe = 1;
	for_each_gfn_indirect_valid_sp(vcpu->kvm, sp, gfn) {
		if (detect_write_misaligned(sp, gpa, bytes) ||
		      detect_write_flooding(sp)) {
			zap_page |= !!kvm_mmu_prepare_zap_page(vcpu->kvm, sp,
						     &invalid_list);
			++vcpu->kvm->stat.mmu_flooded;
			continue;
		}

		spte = get_written_sptes(sp, gpa, &npte);
		if (!spte)
			continue;

		local_flush = true;
		while (npte--) {
			entry = *spte;
			mmu_page_zap_pte(vcpu->kvm, sp, spte);
			if (gentry &&
			      !((sp->role.word ^ vcpu->arch.mmu.base_role.word)
			      & mask.word) && rmap_can_add(vcpu))
				mmu_pte_write_new_pte(vcpu, sp, spte, &gentry);
			if (need_remote_flush(entry, *spte))
				remote_flush = true;
			++spte;
		}
	}
	mmu_pte_write_flush_tlb(vcpu, zap_page, remote_flush, local_flush);
	kvm_mmu_commit_zap_page(vcpu->kvm, &invalid_list);
	kvm_mmu_audit(vcpu, AUDIT_POST_PTE_WRITE);
	spin_unlock(&vcpu->kvm->mmu_lock);
}
