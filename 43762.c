static int mmu_alloc_shadow_roots(struct kvm_vcpu *vcpu)
{
	struct kvm_mmu_page *sp;
	u64 pdptr, pm_mask;
	gfn_t root_gfn;
	int i;

	root_gfn = vcpu->arch.mmu.get_cr3(vcpu) >> PAGE_SHIFT;

	if (mmu_check_root(vcpu, root_gfn))
		return 1;

	/*
	 * Do we shadow a long mode page table? If so we need to
	 * write-protect the guests page table root.
	 */
	if (vcpu->arch.mmu.root_level == PT64_ROOT_LEVEL) {
		hpa_t root = vcpu->arch.mmu.root_hpa;

		ASSERT(!VALID_PAGE(root));

		spin_lock(&vcpu->kvm->mmu_lock);
		make_mmu_pages_available(vcpu);
		sp = kvm_mmu_get_page(vcpu, root_gfn, 0, PT64_ROOT_LEVEL,
				      0, ACC_ALL, NULL);
		root = __pa(sp->spt);
		++sp->root_count;
		spin_unlock(&vcpu->kvm->mmu_lock);
		vcpu->arch.mmu.root_hpa = root;
		return 0;
	}

	/*
	 * We shadow a 32 bit page table. This may be a legacy 2-level
	 * or a PAE 3-level page table. In either case we need to be aware that
	 * the shadow page table may be a PAE or a long mode page table.
	 */
	pm_mask = PT_PRESENT_MASK;
	if (vcpu->arch.mmu.shadow_root_level == PT64_ROOT_LEVEL)
		pm_mask |= PT_ACCESSED_MASK | PT_WRITABLE_MASK | PT_USER_MASK;

	for (i = 0; i < 4; ++i) {
		hpa_t root = vcpu->arch.mmu.pae_root[i];

		ASSERT(!VALID_PAGE(root));
		if (vcpu->arch.mmu.root_level == PT32E_ROOT_LEVEL) {
			pdptr = vcpu->arch.mmu.get_pdptr(vcpu, i);
			if (!is_present_gpte(pdptr)) {
				vcpu->arch.mmu.pae_root[i] = 0;
				continue;
			}
			root_gfn = pdptr >> PAGE_SHIFT;
			if (mmu_check_root(vcpu, root_gfn))
				return 1;
		}
		spin_lock(&vcpu->kvm->mmu_lock);
		make_mmu_pages_available(vcpu);
		sp = kvm_mmu_get_page(vcpu, root_gfn, i << 30,
				      PT32_ROOT_LEVEL, 0,
				      ACC_ALL, NULL);
		root = __pa(sp->spt);
		++sp->root_count;
		spin_unlock(&vcpu->kvm->mmu_lock);

		vcpu->arch.mmu.pae_root[i] = root | pm_mask;
	}
	vcpu->arch.mmu.root_hpa = __pa(vcpu->arch.mmu.pae_root);

	/*
	 * If we shadow a 32 bit page table with a long mode page
	 * table we enter this path.
	 */
	if (vcpu->arch.mmu.shadow_root_level == PT64_ROOT_LEVEL) {
		if (vcpu->arch.mmu.lm_root == NULL) {
			/*
			 * The additional page necessary for this is only
			 * allocated on demand.
			 */

			u64 *lm_root;

			lm_root = (void*)get_zeroed_page(GFP_KERNEL);
			if (lm_root == NULL)
				return 1;

			lm_root[0] = __pa(vcpu->arch.mmu.pae_root) | pm_mask;

			vcpu->arch.mmu.lm_root = lm_root;
		}

		vcpu->arch.mmu.root_hpa = __pa(vcpu->arch.mmu.lm_root);
	}

	return 0;
}
