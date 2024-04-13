static int kvm_handle_bad_page(struct kvm_vcpu *vcpu, gfn_t gfn, pfn_t pfn)
{
	/*
	 * Do not cache the mmio info caused by writing the readonly gfn
	 * into the spte otherwise read access on readonly gfn also can
	 * caused mmio page fault and treat it as mmio access.
	 * Return 1 to tell kvm to emulate it.
	 */
	if (pfn == KVM_PFN_ERR_RO_FAULT)
		return 1;

	if (pfn == KVM_PFN_ERR_HWPOISON) {
		kvm_send_hwpoison_signal(gfn_to_hva(vcpu->kvm, gfn), current);
		return 0;
	}

	return -EFAULT;
}
