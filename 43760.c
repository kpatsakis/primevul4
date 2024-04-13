static int mmu_alloc_direct_roots(struct kvm_vcpu *vcpu)
{
	struct kvm_mmu_page *sp;
	unsigned i;

	if (vcpu->arch.mmu.shadow_root_level == PT64_ROOT_LEVEL) {
		spin_lock(&vcpu->kvm->mmu_lock);
		make_mmu_pages_available(vcpu);
		sp = kvm_mmu_get_page(vcpu, 0, 0, PT64_ROOT_LEVEL,
				      1, ACC_ALL, NULL);
		++sp->root_count;
		spin_unlock(&vcpu->kvm->mmu_lock);
		vcpu->arch.mmu.root_hpa = __pa(sp->spt);
	} else if (vcpu->arch.mmu.shadow_root_level == PT32E_ROOT_LEVEL) {
		for (i = 0; i < 4; ++i) {
			hpa_t root = vcpu->arch.mmu.pae_root[i];

			ASSERT(!VALID_PAGE(root));
			spin_lock(&vcpu->kvm->mmu_lock);
			make_mmu_pages_available(vcpu);
			sp = kvm_mmu_get_page(vcpu, i << (30 - PAGE_SHIFT),
					      i << 30,
					      PT32_ROOT_LEVEL, 1, ACC_ALL,
					      NULL);
			root = __pa(sp->spt);
			++sp->root_count;
			spin_unlock(&vcpu->kvm->mmu_lock);
			vcpu->arch.mmu.pae_root[i] = root | PT_PRESENT_MASK;
		}
		vcpu->arch.mmu.root_hpa = __pa(vcpu->arch.mmu.pae_root);
	} else
		BUG();

	return 0;
}
