void kvm_arch_commit_memory_region(struct kvm *kvm,
				struct kvm_userspace_memory_region *mem,
				const struct kvm_memory_slot *old,
				enum kvm_mr_change change)
{

	int nr_mmu_pages = 0;

	if ((mem->slot >= KVM_USER_MEM_SLOTS) && (change == KVM_MR_DELETE)) {
		int ret;

		ret = vm_munmap(old->userspace_addr,
				old->npages * PAGE_SIZE);
		if (ret < 0)
			printk(KERN_WARNING
			       "kvm_vm_ioctl_set_memory_region: "
			       "failed to munmap memory\n");
	}

	if (!kvm->arch.n_requested_mmu_pages)
		nr_mmu_pages = kvm_mmu_calculate_mmu_pages(kvm);

	if (nr_mmu_pages)
		kvm_mmu_change_mmu_pages(kvm, nr_mmu_pages);
	/*
	 * Write protect all pages for dirty logging.
	 *
	 * All the sptes including the large sptes which point to this
	 * slot are set to readonly. We can not create any new large
	 * spte on this slot until the end of the logging.
	 *
	 * See the comments in fast_page_fault().
	 */
	if ((change != KVM_MR_DELETE) && (mem->flags & KVM_MEM_LOG_DIRTY_PAGES))
		kvm_mmu_slot_remove_write_access(kvm, mem->slot);
}
