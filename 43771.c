static int mmu_need_write_protect(struct kvm_vcpu *vcpu, gfn_t gfn,
				  bool can_unsync)
{
	struct kvm_mmu_page *s;
	bool need_unsync = false;

	for_each_gfn_indirect_valid_sp(vcpu->kvm, s, gfn) {
		if (!can_unsync)
			return 1;

		if (s->role.level != PT_PAGE_TABLE_LEVEL)
			return 1;

		if (!s->unsync)
			need_unsync = true;
	}
	if (need_unsync)
		kvm_unsync_pages(vcpu, gfn);
	return 0;
}
