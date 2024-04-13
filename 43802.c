static bool page_fault_can_be_fast(struct kvm_vcpu *vcpu, u32 error_code)
{
	/*
	 * Do not fix the mmio spte with invalid generation number which
	 * need to be updated by slow page fault path.
	 */
	if (unlikely(error_code & PFERR_RSVD_MASK))
		return false;

	/*
	 * #PF can be fast only if the shadow page table is present and it
	 * is caused by write-protect, that means we just need change the
	 * W bit of the spte which can be done out of mmu-lock.
	 */
	if (!(error_code & PFERR_PRESENT_MASK) ||
	      !(error_code & PFERR_WRITE_MASK))
		return false;

	return true;
}
