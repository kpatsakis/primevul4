static void walk_shadow_page_lockless_begin(struct kvm_vcpu *vcpu)
{
	/*
	 * Prevent page table teardown by making any free-er wait during
	 * kvm_flush_remote_tlbs() IPI to all active vcpus.
	 */
	local_irq_disable();
	vcpu->mode = READING_SHADOW_PAGE_TABLES;
	/*
	 * Make sure a following spte read is not reordered ahead of the write
	 * to vcpu->mode.
	 */
	smp_mb();
}
