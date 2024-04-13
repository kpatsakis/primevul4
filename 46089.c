int kvm_vm_ioctl_get_dirty_log(struct kvm *kvm,
				      struct kvm_dirty_log *log)
{
	int r, i;
	struct kvm_memory_slot *memslot;
	unsigned long n;
	unsigned long is_dirty = 0;

	mutex_lock(&kvm->slots_lock);

	r = -EINVAL;
	if (log->slot >= KVM_MEMORY_SLOTS)
		goto out;

	memslot = &kvm->memslots->memslots[log->slot];
	r = -ENOENT;
	if (!memslot->dirty_bitmap)
		goto out;

	n = kvm_dirty_bitmap_bytes(memslot);

	for (i = 0; !is_dirty && i < n/sizeof(long); i++)
		is_dirty = memslot->dirty_bitmap[i];

	/* If nothing is dirty, don't bother messing with page tables. */
	if (is_dirty) {
		struct kvm_memslots *slots, *old_slots;
		unsigned long *dirty_bitmap;

		dirty_bitmap = memslot->dirty_bitmap_head;
		if (memslot->dirty_bitmap == dirty_bitmap)
			dirty_bitmap += n / sizeof(long);
		memset(dirty_bitmap, 0, n);

		r = -ENOMEM;
		slots = kzalloc(sizeof(struct kvm_memslots), GFP_KERNEL);
		if (!slots)
			goto out;
		memcpy(slots, kvm->memslots, sizeof(struct kvm_memslots));
		slots->memslots[log->slot].dirty_bitmap = dirty_bitmap;
		slots->generation++;

		old_slots = kvm->memslots;
		rcu_assign_pointer(kvm->memslots, slots);
		synchronize_srcu_expedited(&kvm->srcu);
		dirty_bitmap = old_slots->memslots[log->slot].dirty_bitmap;
		kfree(old_slots);

		spin_lock(&kvm->mmu_lock);
		kvm_mmu_slot_remove_write_access(kvm, log->slot);
		spin_unlock(&kvm->mmu_lock);

		r = -EFAULT;
		if (copy_to_user(log->dirty_bitmap, dirty_bitmap, n))
			goto out;
	} else {
		r = -EFAULT;
		if (clear_user(log->dirty_bitmap, n))
			goto out;
	}

	r = 0;
out:
	mutex_unlock(&kvm->slots_lock);
	return r;
}
