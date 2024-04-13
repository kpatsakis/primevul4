int kvm_arch_init(void *opaque)
{
	int r;
	struct kvm_x86_ops *ops = opaque;

	if (kvm_x86_ops) {
		printk(KERN_ERR "kvm: already loaded the other module\n");
		r = -EEXIST;
		goto out;
	}

	if (!ops->cpu_has_kvm_support()) {
		printk(KERN_ERR "kvm: no hardware support\n");
		r = -EOPNOTSUPP;
		goto out;
	}
	if (ops->disabled_by_bios()) {
		printk(KERN_ERR "kvm: disabled by bios\n");
		r = -EOPNOTSUPP;
		goto out;
	}

	r = -ENOMEM;
	shared_msrs = alloc_percpu(struct kvm_shared_msrs);
	if (!shared_msrs) {
		printk(KERN_ERR "kvm: failed to allocate percpu kvm_shared_msrs\n");
		goto out;
	}

	r = kvm_mmu_module_init();
	if (r)
		goto out_free_percpu;

	kvm_set_mmio_spte_mask();

	kvm_x86_ops = ops;
	kvm_init_msr_list();

	kvm_mmu_set_mask_ptes(PT_USER_MASK, PT_ACCESSED_MASK,
			PT_DIRTY_MASK, PT64_NX_MASK, 0);

	kvm_timer_init();

	perf_register_guest_info_callbacks(&kvm_guest_cbs);

	if (cpu_has_xsave)
		host_xcr0 = xgetbv(XCR_XFEATURE_ENABLED_MASK);

	kvm_lapic_init();
#ifdef CONFIG_X86_64
	pvclock_gtod_register_notifier(&pvclock_gtod_notifier);
#endif

	return 0;

out_free_percpu:
	free_percpu(shared_msrs);
out:
	return r;
}
