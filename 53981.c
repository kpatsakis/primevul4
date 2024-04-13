void kvm_vcpu_reload_apic_access_page(struct kvm_vcpu *vcpu)
{
	struct page *page = NULL;

	if (!lapic_in_kernel(vcpu))
		return;

	if (!kvm_x86_ops->set_apic_access_page_addr)
		return;

	page = gfn_to_page(vcpu->kvm, APIC_DEFAULT_PHYS_BASE >> PAGE_SHIFT);
	if (is_error_page(page))
		return;
	kvm_x86_ops->set_apic_access_page_addr(vcpu, page_to_phys(page));

	/*
	 * Do not pin apic access page in memory, the MMU notifier
	 * will call us again if it is migrated or swapped out.
	 */
	put_page(page);
}
