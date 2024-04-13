static int __direct_map(struct kvm_vcpu *vcpu, gpa_t v, int write,
			int map_writable, int level, gfn_t gfn, pfn_t pfn,
			bool prefault)
{
	struct kvm_shadow_walk_iterator iterator;
	struct kvm_mmu_page *sp;
	int emulate = 0;
	gfn_t pseudo_gfn;

	for_each_shadow_entry(vcpu, (u64)gfn << PAGE_SHIFT, iterator) {
		if (iterator.level == level) {
			mmu_set_spte(vcpu, iterator.sptep, ACC_ALL,
				     write, &emulate, level, gfn, pfn,
				     prefault, map_writable);
			direct_pte_prefetch(vcpu, iterator.sptep);
			++vcpu->stat.pf_fixed;
			break;
		}

		if (!is_shadow_present_pte(*iterator.sptep)) {
			u64 base_addr = iterator.addr;

			base_addr &= PT64_LVL_ADDR_MASK(iterator.level);
			pseudo_gfn = base_addr >> PAGE_SHIFT;
			sp = kvm_mmu_get_page(vcpu, pseudo_gfn, iterator.addr,
					      iterator.level - 1,
					      1, ACC_ALL, iterator.sptep);

			link_shadow_page(iterator.sptep, sp, true);
		}
	}
	return emulate;
}
