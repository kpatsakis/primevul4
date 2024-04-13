int kvm_vm_ioctl_get_dirty_log(struct kvm *kvm, struct kvm_dirty_log *log)
{
	bool is_dirty = false;
	int r;

	mutex_lock(&kvm->slots_lock);

	/*
	 * Flush potentially hardware-cached dirty pages to dirty_bitmap.
	 */
	if (kvm_x86_ops->flush_log_dirty)
		kvm_x86_ops->flush_log_dirty(kvm);

	r = kvm_get_dirty_log_protect(kvm, log, &is_dirty);

	/*
	 * All the TLBs can be flushed out of mmu lock, see the comments in
	 * kvm_mmu_slot_remove_write_access().
	 */
	lockdep_assert_held(&kvm->slots_lock);
	if (is_dirty)
		kvm_flush_remote_tlbs(kvm);

	mutex_unlock(&kvm->slots_lock);
	return r;
}
