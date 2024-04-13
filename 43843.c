static int alloc_identity_pagetable(struct kvm *kvm)
{
	struct page *page;
	struct kvm_userspace_memory_region kvm_userspace_mem;
	int r = 0;

	mutex_lock(&kvm->slots_lock);
	if (kvm->arch.ept_identity_pagetable)
		goto out;
	kvm_userspace_mem.slot = IDENTITY_PAGETABLE_PRIVATE_MEMSLOT;
	kvm_userspace_mem.flags = 0;
	kvm_userspace_mem.guest_phys_addr =
		kvm->arch.ept_identity_map_addr;
	kvm_userspace_mem.memory_size = PAGE_SIZE;
	r = __kvm_set_memory_region(kvm, &kvm_userspace_mem);
	if (r)
		goto out;

	page = gfn_to_page(kvm, kvm->arch.ept_identity_map_addr >> PAGE_SHIFT);
	if (is_error_page(page)) {
		r = -EFAULT;
		goto out;
	}

	kvm->arch.ept_identity_pagetable = page;
out:
	mutex_unlock(&kvm->slots_lock);
	return r;
}
