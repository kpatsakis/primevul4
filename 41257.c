int kvm_init(void *opaque, unsigned vcpu_size, unsigned vcpu_align,
		  struct module *module)
{
	int r;
	int cpu;

	r = kvm_arch_init(opaque);
	if (r)
		goto out_fail;

	bad_page = alloc_page(GFP_KERNEL | __GFP_ZERO);

	if (bad_page == NULL) {
		r = -ENOMEM;
		goto out;
	}

	bad_pfn = page_to_pfn(bad_page);

	hwpoison_page = alloc_page(GFP_KERNEL | __GFP_ZERO);

	if (hwpoison_page == NULL) {
		r = -ENOMEM;
		goto out_free_0;
	}

	hwpoison_pfn = page_to_pfn(hwpoison_page);

	fault_page = alloc_page(GFP_KERNEL | __GFP_ZERO);

	if (fault_page == NULL) {
		r = -ENOMEM;
		goto out_free_0;
	}

	fault_pfn = page_to_pfn(fault_page);

	if (!zalloc_cpumask_var(&cpus_hardware_enabled, GFP_KERNEL)) {
		r = -ENOMEM;
		goto out_free_0;
	}

	r = kvm_arch_hardware_setup();
	if (r < 0)
		goto out_free_0a;

	for_each_online_cpu(cpu) {
		smp_call_function_single(cpu,
				kvm_arch_check_processor_compat,
				&r, 1);
		if (r < 0)
			goto out_free_1;
	}

	r = register_cpu_notifier(&kvm_cpu_notifier);
	if (r)
		goto out_free_2;
	register_reboot_notifier(&kvm_reboot_notifier);

	/* A kmem cache lets us meet the alignment requirements of fx_save. */
	if (!vcpu_align)
		vcpu_align = __alignof__(struct kvm_vcpu);
	kvm_vcpu_cache = kmem_cache_create("kvm_vcpu", vcpu_size, vcpu_align,
					   0, NULL);
	if (!kvm_vcpu_cache) {
		r = -ENOMEM;
		goto out_free_3;
	}

	r = kvm_async_pf_init();
	if (r)
		goto out_free;

	kvm_chardev_ops.owner = module;
	kvm_vm_fops.owner = module;
	kvm_vcpu_fops.owner = module;

	r = misc_register(&kvm_dev);
	if (r) {
		printk(KERN_ERR "kvm: misc device register failed\n");
		goto out_unreg;
	}

	register_syscore_ops(&kvm_syscore_ops);

	kvm_preempt_ops.sched_in = kvm_sched_in;
	kvm_preempt_ops.sched_out = kvm_sched_out;

	kvm_init_debug();

	return 0;

out_unreg:
	kvm_async_pf_deinit();
out_free:
	kmem_cache_destroy(kvm_vcpu_cache);
out_free_3:
	unregister_reboot_notifier(&kvm_reboot_notifier);
	unregister_cpu_notifier(&kvm_cpu_notifier);
out_free_2:
out_free_1:
	kvm_arch_hardware_unsetup();
out_free_0a:
	free_cpumask_var(cpus_hardware_enabled);
out_free_0:
	if (fault_page)
		__free_page(fault_page);
	if (hwpoison_page)
		__free_page(hwpoison_page);
	__free_page(bad_page);
out:
	kvm_arch_exit();
out_fail:
	return r;
}
