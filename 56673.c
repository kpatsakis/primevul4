static void vmx_vcpu_load(struct kvm_vcpu *vcpu, int cpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	bool already_loaded = vmx->loaded_vmcs->cpu == cpu;

	if (!already_loaded) {
		loaded_vmcs_clear(vmx->loaded_vmcs);
		local_irq_disable();
		crash_disable_local_vmclear(cpu);

		/*
		 * Read loaded_vmcs->cpu should be before fetching
		 * loaded_vmcs->loaded_vmcss_on_cpu_link.
		 * See the comments in __loaded_vmcs_clear().
		 */
		smp_rmb();

		list_add(&vmx->loaded_vmcs->loaded_vmcss_on_cpu_link,
			 &per_cpu(loaded_vmcss_on_cpu, cpu));
		crash_enable_local_vmclear(cpu);
		local_irq_enable();
	}

	if (per_cpu(current_vmcs, cpu) != vmx->loaded_vmcs->vmcs) {
		per_cpu(current_vmcs, cpu) = vmx->loaded_vmcs->vmcs;
		vmcs_load(vmx->loaded_vmcs->vmcs);
	}

	if (!already_loaded) {
		void *gdt = get_current_gdt_ro();
		unsigned long sysenter_esp;

		kvm_make_request(KVM_REQ_TLB_FLUSH, vcpu);

		/*
		 * Linux uses per-cpu TSS and GDT, so set these when switching
		 * processors.  See 22.2.4.
		 */
		vmcs_writel(HOST_TR_BASE,
			    (unsigned long)this_cpu_ptr(&cpu_tss));
		vmcs_writel(HOST_GDTR_BASE, (unsigned long)gdt);   /* 22.2.4 */

		/*
		 * VM exits change the host TR limit to 0x67 after a VM
		 * exit.  This is okay, since 0x67 covers everything except
		 * the IO bitmap and have have code to handle the IO bitmap
		 * being lost after a VM exit.
		 */
		BUILD_BUG_ON(IO_BITMAP_OFFSET - 1 != 0x67);

		rdmsrl(MSR_IA32_SYSENTER_ESP, sysenter_esp);
		vmcs_writel(HOST_IA32_SYSENTER_ESP, sysenter_esp); /* 22.2.3 */

		vmx->loaded_vmcs->cpu = cpu;
	}

	/* Setup TSC multiplier */
	if (kvm_has_tsc_control &&
	    vmx->current_tsc_ratio != vcpu->arch.tsc_scaling_ratio)
		decache_tsc_multiplier(vmx);

	vmx_vcpu_pi_load(vcpu, cpu);
	vmx->host_pkru = read_pkru();
}
