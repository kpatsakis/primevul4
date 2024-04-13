static void vmx_enable_log_dirty_pt_masked(struct kvm *kvm,
					   struct kvm_memory_slot *memslot,
					   gfn_t offset, unsigned long mask)
{
	kvm_mmu_clear_dirty_pt_masked(kvm, memslot, offset, mask);
}
