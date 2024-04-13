static void vmx_slot_enable_log_dirty(struct kvm *kvm,
				     struct kvm_memory_slot *slot)
{
	kvm_mmu_slot_leaf_clear_dirty(kvm, slot);
	kvm_mmu_slot_largepage_remove_write_access(kvm, slot);
}
