static int init_rmode_identity_map(struct kvm *kvm)
{
	int i, idx, r, ret;
	pfn_t identity_map_pfn;
	u32 tmp;

	if (!enable_ept)
		return 1;
	if (unlikely(!kvm->arch.ept_identity_pagetable)) {
		printk(KERN_ERR "EPT: identity-mapping pagetable "
			"haven't been allocated!\n");
		return 0;
	}
	if (likely(kvm->arch.ept_identity_pagetable_done))
		return 1;
	ret = 0;
	identity_map_pfn = kvm->arch.ept_identity_map_addr >> PAGE_SHIFT;
	idx = srcu_read_lock(&kvm->srcu);
	r = kvm_clear_guest_page(kvm, identity_map_pfn, 0, PAGE_SIZE);
	if (r < 0)
		goto out;
	/* Set up identity-mapping pagetable for EPT in real mode */
	for (i = 0; i < PT32_ENT_PER_PAGE; i++) {
		tmp = (i << 22) + (_PAGE_PRESENT | _PAGE_RW | _PAGE_USER |
			_PAGE_ACCESSED | _PAGE_DIRTY | _PAGE_PSE);
		r = kvm_write_guest_page(kvm, identity_map_pfn,
				&tmp, i * sizeof(tmp), sizeof(tmp));
		if (r < 0)
			goto out;
	}
	kvm->arch.ept_identity_pagetable_done = true;
	ret = 1;
out:
	srcu_read_unlock(&kvm->srcu, idx);
	return ret;
}
