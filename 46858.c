static int hardware_enable(void)
{
	int cpu = raw_smp_processor_id();
	u64 phys_addr = __pa(per_cpu(vmxarea, cpu));
	u64 old, test_bits;

	if (cr4_read_shadow() & X86_CR4_VMXE)
		return -EBUSY;

	INIT_LIST_HEAD(&per_cpu(loaded_vmcss_on_cpu, cpu));
	INIT_LIST_HEAD(&per_cpu(blocked_vcpu_on_cpu, cpu));
	spin_lock_init(&per_cpu(blocked_vcpu_on_cpu_lock, cpu));

	/*
	 * Now we can enable the vmclear operation in kdump
	 * since the loaded_vmcss_on_cpu list on this cpu
	 * has been initialized.
	 *
	 * Though the cpu is not in VMX operation now, there
	 * is no problem to enable the vmclear operation
	 * for the loaded_vmcss_on_cpu list is empty!
	 */
	crash_enable_local_vmclear(cpu);

	rdmsrl(MSR_IA32_FEATURE_CONTROL, old);

	test_bits = FEATURE_CONTROL_LOCKED;
	test_bits |= FEATURE_CONTROL_VMXON_ENABLED_OUTSIDE_SMX;
	if (tboot_enabled())
		test_bits |= FEATURE_CONTROL_VMXON_ENABLED_INSIDE_SMX;

	if ((old & test_bits) != test_bits) {
		/* enable and lock */
		wrmsrl(MSR_IA32_FEATURE_CONTROL, old | test_bits);
	}
	cr4_set_bits(X86_CR4_VMXE);

	if (vmm_exclusive) {
		kvm_cpu_vmxon(phys_addr);
		ept_sync_global();
	}

	native_store_gdt(this_cpu_ptr(&host_gdt));

	return 0;
}
