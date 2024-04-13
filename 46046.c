void kvm_arch_async_page_ready(struct kvm_vcpu *vcpu, struct kvm_async_pf *work)
{
	int r;

	if (!vcpu->arch.mmu.direct_map || !work->arch.direct_map ||
	      is_error_page(work->page))
		return;

	r = kvm_mmu_reload(vcpu);
	if (unlikely(r))
		return;

	vcpu->arch.mmu.page_fault(vcpu, work->gva, 0, true);
}
