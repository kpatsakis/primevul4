static void kvm_unsync_pages(struct kvm_vcpu *vcpu,  gfn_t gfn)
{
	struct kvm_mmu_page *s;

	for_each_gfn_indirect_valid_sp(vcpu->kvm, s, gfn) {
		if (s->unsync)
			continue;
		WARN_ON(s->role.level != PT_PAGE_TABLE_LEVEL);
		__kvm_unsync_page(vcpu, s);
	}
}
