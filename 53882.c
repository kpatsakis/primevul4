int __x86_set_memory_region(struct kvm *kvm, int id, gpa_t gpa, u32 size)
{
	int i, r;
	unsigned long hva;
	struct kvm_memslots *slots = kvm_memslots(kvm);
	struct kvm_memory_slot *slot, old;

	/* Called with kvm->slots_lock held.  */
	if (WARN_ON(id >= KVM_MEM_SLOTS_NUM))
		return -EINVAL;

	slot = id_to_memslot(slots, id);
	if (size) {
		if (WARN_ON(slot->npages))
			return -EEXIST;

		/*
		 * MAP_SHARED to prevent internal slot pages from being moved
		 * by fork()/COW.
		 */
		hva = vm_mmap(NULL, 0, size, PROT_READ | PROT_WRITE,
			      MAP_SHARED | MAP_ANONYMOUS, 0);
		if (IS_ERR((void *)hva))
			return PTR_ERR((void *)hva);
	} else {
		if (!slot->npages)
			return 0;

		hva = 0;
	}

	old = *slot;
	for (i = 0; i < KVM_ADDRESS_SPACE_NUM; i++) {
		struct kvm_userspace_memory_region m;

		m.slot = id | (i << 16);
		m.flags = 0;
		m.guest_phys_addr = gpa;
		m.userspace_addr = hva;
		m.memory_size = size;
		r = __kvm_set_memory_region(kvm, &m);
		if (r < 0)
			return r;
	}

	if (!size) {
		r = vm_munmap(old.userspace_addr, old.npages * PAGE_SIZE);
		WARN_ON(r < 0);
	}

	return 0;
}
