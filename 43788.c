static void mmu_sync_roots(struct kvm_vcpu *vcpu)
{
	int i;
	struct kvm_mmu_page *sp;

	if (vcpu->arch.mmu.direct_map)
		return;

	if (!VALID_PAGE(vcpu->arch.mmu.root_hpa))
		return;

	vcpu_clear_mmio_info(vcpu, ~0ul);
	kvm_mmu_audit(vcpu, AUDIT_PRE_SYNC);
	if (vcpu->arch.mmu.root_level == PT64_ROOT_LEVEL) {
		hpa_t root = vcpu->arch.mmu.root_hpa;
		sp = page_header(root);
		mmu_sync_children(vcpu, sp);
		kvm_mmu_audit(vcpu, AUDIT_POST_SYNC);
		return;
	}
	for (i = 0; i < 4; ++i) {
		hpa_t root = vcpu->arch.mmu.pae_root[i];

		if (root && VALID_PAGE(root)) {
			root &= PT64_BASE_ADDR_MASK;
			sp = page_header(root);
			mmu_sync_children(vcpu, sp);
		}
	}
	kvm_mmu_audit(vcpu, AUDIT_POST_SYNC);
}
