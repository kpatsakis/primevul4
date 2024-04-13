void kvm_mmu_invalidate_mmio_sptes(struct kvm *kvm)
{
	/*
	 * The very rare case: if the generation-number is round,
	 * zap all shadow pages.
	 */
	if (unlikely(kvm_current_mmio_generation(kvm) >= MMIO_MAX_GEN)) {
		printk_ratelimited(KERN_INFO "kvm: zapping shadow pages for mmio generation wraparound\n");
		kvm_mmu_invalidate_zap_all_pages(kvm);
	}
}
