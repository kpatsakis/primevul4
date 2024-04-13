static struct kvm_mmu_page *kvm_mmu_get_page(struct kvm_vcpu *vcpu,
					     gfn_t gfn,
					     gva_t gaddr,
					     unsigned level,
					     int direct,
					     unsigned access,
					     u64 *parent_pte)
{
	union kvm_mmu_page_role role;
	unsigned quadrant;
	struct kvm_mmu_page *sp;
	bool need_sync = false;

	role = vcpu->arch.mmu.base_role;
	role.level = level;
	role.direct = direct;
	if (role.direct)
		role.cr4_pae = 0;
	role.access = access;
	if (!vcpu->arch.mmu.direct_map
	    && vcpu->arch.mmu.root_level <= PT32_ROOT_LEVEL) {
		quadrant = gaddr >> (PAGE_SHIFT + (PT64_PT_BITS * level));
		quadrant &= (1 << ((PT32_PT_BITS - PT64_PT_BITS) * level)) - 1;
		role.quadrant = quadrant;
	}
	for_each_gfn_sp(vcpu->kvm, sp, gfn) {
		if (is_obsolete_sp(vcpu->kvm, sp))
			continue;

		if (!need_sync && sp->unsync)
			need_sync = true;

		if (sp->role.word != role.word)
			continue;

		if (sp->unsync && kvm_sync_page_transient(vcpu, sp))
			break;

		mmu_page_add_parent_pte(vcpu, sp, parent_pte);
		if (sp->unsync_children) {
			kvm_make_request(KVM_REQ_MMU_SYNC, vcpu);
			kvm_mmu_mark_parents_unsync(sp);
		} else if (sp->unsync)
			kvm_mmu_mark_parents_unsync(sp);

		__clear_sp_write_flooding_count(sp);
		trace_kvm_mmu_get_page(sp, false);
		return sp;
	}
	++vcpu->kvm->stat.mmu_cache_miss;
	sp = kvm_mmu_alloc_page(vcpu, parent_pte, direct);
	if (!sp)
		return sp;
	sp->gfn = gfn;
	sp->role = role;
	hlist_add_head(&sp->hash_link,
		&vcpu->kvm->arch.mmu_page_hash[kvm_page_table_hashfn(gfn)]);
	if (!direct) {
		if (rmap_write_protect(vcpu->kvm, gfn))
			kvm_flush_remote_tlbs(vcpu->kvm);
		if (level > PT_PAGE_TABLE_LEVEL && need_sync)
			kvm_sync_pages(vcpu, gfn);

		account_shadowed(vcpu->kvm, gfn);
	}
	sp->mmu_valid_gen = vcpu->kvm->arch.mmu_valid_gen;
	init_shadow_page_table(sp);
	trace_kvm_mmu_get_page(sp, true);
	return sp;
}
